#include "storage.h"

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "node.h"
#include "stb_ds.h"
#include "utils.h"

TableDef* TableDefs = NULL;
Page** TablePages = NULL;  // 2d stb array.
RelStorageManager* SMS = NULL;

// A hardcoded table definition for the system catalog table that contains table definitions
// for all other tables.
static TableDef RelCatalogTableDef = {.name = "reltabledef", .tuple_desc = NULL, .index = 0};

// A hardcoded table def for the system catalog that stores index definitions.
static TableDef IndexCatalogTableDef = {
    .name = "indexcatalogtabledef", .tuple_desc = NULL, .index = 0};

// stb array that contains the materialized index definitions from the system catalog.
static IndexDef* IndexDefs = NULL;

Tuple* MakeTuple(const TableDef* table_def) {
  assert(table_def != NULL);
  assert(table_def->tuple_desc != NULL);
  size_t num_cols = arrlenu(table_def->tuple_desc);
  size_t tuple_length = sizeof(Tuple) + num_cols * sizeof(byte) + num_cols * sizeof(DataLoc) +
                        num_cols * sizeof(byte);
  Tuple* t = calloc(1, tuple_length);
  t->length = tuple_length;
  t->num_cols = num_cols;
  for (size_t i = 0; i < t->num_cols; ++i) {
    t->null_bitmap[i] |= 1;
  }

  DataLoc* data_locs = GetDataLocs(t);
  size_t cur_offset = 0;
  for (size_t i = 0; i < t->num_cols; ++i) {
    data_locs[i].offset = cur_offset;
    data_locs[i].length = sizeof(byte);
    cur_offset += sizeof(byte);
  }
  return t;
}

void InitSystemTables() {
  // Initialize the table def for system catalog that holds table def info.
  ColDesc* reltabledef_col_desc = NULL;
  arrpush(reltabledef_col_desc, ((ColDesc){.column_name = "name", .type = T_STRING}));
  arrpush(reltabledef_col_desc, ((ColDesc){.column_name = "index", .type = T_INT}));
  arrpush(reltabledef_col_desc, ((ColDesc){.column_name = "columns", .type = T_STRING}));
  RelCatalogTableDef.tuple_desc = reltabledef_col_desc;

  // Initialize tuple desc for the system catalog that stores index definitions.
  ColDesc* indexcatalog_col_desc = NULL;
  arrpush(indexcatalog_col_desc, ((ColDesc){.column_name = "index_id", .type = T_INT}));
  arrpush(indexcatalog_col_desc, ((ColDesc){.column_name = "index_name", .type = T_STRING}));
  arrpush(indexcatalog_col_desc, ((ColDesc){.column_name = "col_idxs", .type = T_STRING}));
  arrpush(indexcatalog_col_desc, ((ColDesc){.column_name = "table_def_idx", .type = T_INT}));
  arrpush(indexcatalog_col_desc,
          ((ColDesc){.column_name = "index_table_def_idx", .type = T_INT}));
  IndexCatalogTableDef.tuple_desc = indexcatalog_col_desc;

  FILE* file = fopen("data_dir/reltabledef", "r");
  // If system catalog doesn't already exist, create it along with other default tables.
  if (file == NULL) {
    // Save system catalogs to disk.
    CreateTable(&RelCatalogTableDef);
    CreateTable(&IndexCatalogTableDef);

    ColDesc* tuple_desc = NULL;
    arrpush(tuple_desc, ((ColDesc){.column_name = "bar", .type = T_STRING}));
    arrpush(tuple_desc, ((ColDesc){.column_name = "baz", .type = T_BOOL}));
    TableDef table_def = {.name = "foo", .tuple_desc = tuple_desc};
    CreateTable(&table_def);

    Tuple* t1 = MakeTuple(&table_def);
    t1 = SetCol(t1, "bar", MakeDatum(T_STRING, strdup("hello")), &table_def);
    bool* bool_lit = (bool*)calloc(sizeof(bool), 1);
    *bool_lit = true;
    t1 = SetCol(t1, "baz", MakeDatum(T_BOOL, bool_lit), &table_def);

    Tuple* t2 = MakeTuple(&table_def);
    t2 = SetCol(t2, "bar", MakeDatum(T_STRING, strdup("world")), &table_def);
    bool_lit = (bool*)calloc(sizeof(bool), 1);
    *bool_lit = false;
    t2 = SetCol(t2, "baz", MakeDatum(T_BOOL, bool_lit), &table_def);

    Cursor cursor;
    CursorInit(&cursor, &table_def);
    CursorInsertTuple(&cursor, t1);
    CursorInsertTuple(&cursor, t2);

    ColDesc* table2_tuple_desc = NULL;
    arrpush(table2_tuple_desc, ((ColDesc){.column_name = "a", .type = T_STRING}));
    TableDef table_def2 = {.name = "b", .tuple_desc = table2_tuple_desc};
    CreateTable(&table_def2);

    CursorInit(&cursor, &table_def2);

    Tuple* table2_t1 = MakeTuple(&table_def2);
    table2_t1 = SetCol(table2_t1, "a", MakeDatum(T_STRING, strdup("asdf")), &table_def2);
    CursorInsertTuple(&cursor, table2_t1);
    Tuple* table2_t2 = MakeTuple(&table_def2);
    table2_t2 = SetCol(table2_t2, "a", MakeDatum(T_STRING, strdup("cab")), &table_def2);
    CursorInsertTuple(&cursor, table2_t2);
  } else {
    // Else, assume system catalog tables + default tables already exist.
    fclose(file);

    // Some of the storage manager code assumes that relations it is trying to read/write to
    // and from are already defined.
    arrpush(TableDefs, RelCatalogTableDef);

    Cursor cursor;
    CursorInit(&cursor, &RelCatalogTableDef);

    // Materialize table defs into memory. A lot of code assumes that all table defs are
    // already materialized. Additionally, we assume that the relcatalog table stores table
    // defs in sorted index/creation order.
    Tuple* tuple = CursorSeekNext(&cursor);
    // NOTE: We assume the first tuple is the rel table def catalog tuple definitions, so
    // overwrite the existing one.
    TableDefs[0] = *DeserializeTableDef(tuple);
    tuple = CursorSeekNext(&cursor);
    while (tuple != NULL) {
      arrpush(TableDefs, *DeserializeTableDef(tuple));
      tuple = CursorSeekNext(&cursor);
    }

    // BIG ASSUMPTION/NOTE: We assume that the table def for the index def system catalog is
    // stored as the second table def entry. Ideally we do a search to find the exact index,
    // but we are being lazy here for now.
    IndexCatalogTableDef = TableDefs[1];

    CursorInit(&cursor, &IndexCatalogTableDef);

    // Materialize index definitions into memory.
    tuple = CursorSeekNext(&cursor);
    while (tuple != NULL) {
      IndexDef index_def;
      DeserializeIndexDef(tuple, &index_def);
      arrpush(IndexDefs, index_def);
      tuple = CursorSeekNext(&cursor);
    }
  }
}

TableDef* MakeTableDef(const char* name, ColDesc* tuple_desc, size_t index) {
  size_t len = strlen(name);
  char* cpy = (char*)calloc(sizeof(char), len);
  strncpy(cpy, name, len);
  TableDef* def = (TableDef*)calloc(sizeof(TableDef), 1);
  def->name = cpy;
  def->tuple_desc = tuple_desc;
  def->index = index;
  return def;
}

TableDef* FindTableDef(const char* table_name) {
  TableDef* table_def = NULL;
  size_t i = 0;
  for (; i < arrlenu(TableDefs); ++i) {
    if (strcmp(TableDefs[i].name, table_name) == 0) {
      table_def = &TableDefs[i];
      break;
    }
  }

  // TODO(ryan): Just return from break and return null with no check here.
  return i == arrlenu(TableDefs) ? NULL : table_def;
}

TableDef* GetTableDef(size_t idx) {
  return idx >= arrlenu(TableDefs) ? NULL : &TableDefs[idx];
}

BType GetColType(TableDef* table_def, const char* col_name) {
  for (size_t i = 0; i < arrlenu(table_def->tuple_desc); ++i) {
    if (strcmp(col_name, table_def->tuple_desc[i].column_name) == 0) {
      return table_def->tuple_desc[i].type;
    }
  }
  return T_UNKNOWN;
}

Tuple* SerializeTableDef(const TableDef* table_def) {
  size_t columns_str_len = 0;
  for (size_t i = 0; i < arrlenu(table_def->tuple_desc); ++i) {
    ColDesc* desc = &table_def->tuple_desc[i];
    columns_str_len +=
        strlen(desc->column_name) + strlen(TypeToString(desc->type)) + strlen(",");
  }
  // Account for ',' between elements. Its one less than number of coldesc elements because
  // there is no trailing ','.
  columns_str_len += arrlenu(table_def->tuple_desc) - 1;
  char* columns_str = calloc(columns_str_len + 1, sizeof(char));
  for (size_t i = 0; i < arrlenu(table_def->tuple_desc); ++i) {
    ColDesc* desc = &table_def->tuple_desc[i];
    columns_str = strcat(columns_str, TypeToString(desc->type));
    columns_str = strcat(columns_str, ",");
    columns_str = strcat(columns_str, desc->column_name);
    if (i < arrlenu(table_def->tuple_desc) - 1) {
      columns_str = strcat(columns_str, ",");
    }
  }

  int32_t* int_lit = (int32_t*)calloc(1, sizeof(int32_t));
  *int_lit = (int32_t)table_def->index;

  Tuple* tuple = MakeTuple(&RelCatalogTableDef);
  tuple =
      SetCol(tuple, "name", MakeDatum(T_STRING, strdup(table_def->name)), &RelCatalogTableDef);
  tuple = SetCol(tuple, "index", MakeDatum(T_INT, int_lit), &RelCatalogTableDef);
  tuple = SetCol(tuple, "columns", MakeDatum(T_STRING, columns_str), &RelCatalogTableDef);
  return tuple;
}

TableDef* DeserializeTableDef(Tuple* table_def_tuple) {
  TableDef* table_def = (TableDef*)calloc(1, sizeof(TableDef));
  table_def->name = strdup(GetCol(table_def_tuple, "name", &RelCatalogTableDef).data);
  int32_t* index_ptr = (int32_t*)GetCol(table_def_tuple, "index", &RelCatalogTableDef).data;
  table_def->index = (size_t)(*index_ptr);
  // Columns are serialized as a string of
  // "col1_type_str,col1_name,...,coln_type_str,coln_name".
  char* save_ptr = NULL;
  char* tok =
      strtok_r(GetCol(table_def_tuple, "columns", &RelCatalogTableDef).data, ",", &save_ptr);
  assert(tok != NULL);
  while (tok != NULL) {
    char* type_str = tok;
    assert(type_str != NULL);
    tok = strtok_r(NULL, ",", &save_ptr);
    char* col_name = tok;
    arrpush(table_def->tuple_desc,
            ((ColDesc){.column_name = strdup(col_name), .type = StringToType(type_str)}));
    tok = strtok_r(NULL, ",", &save_ptr);
  }
  return table_def;
}

size_t GetColIdx(const TableDef* table_def, const char* col_name, bool* is_missing) {
  assert(table_def != NULL);
  assert(table_def->tuple_desc != NULL);
  assert(is_missing != NULL);
  for (size_t i = 0; i < arrlenu(table_def->tuple_desc); ++i) {
    if (strcmp(table_def->tuple_desc[i].column_name, col_name) == 0) {
      *is_missing = false;
      return i;
    }
  }

  *is_missing = true;
  return 0;
}

Datum GetCol(Tuple* tuple, const char* col_name, const TableDef* table_def) {
  assert(tuple != NULL);
  assert(table_def != NULL);
  assert(table_def->tuple_desc != NULL);

  bool is_missing = false;
  size_t i = GetColIdx(table_def, col_name, &is_missing);
  assert(!is_missing);

  if (tuple->null_bitmap[i]) {
    return MakeDatum(T_NULL, NULL);
  }

  DataLoc* locs = GetDataLocs(tuple);
  byte* data = GetDataPtr(tuple) + locs[i].offset;
  Datum datum = MakeDatum(table_def->tuple_desc[i].type, data);
  datum.length = locs[i].length;
  return datum;
}

Datum GetColByIdx(Tuple* tuple, size_t idx, const TableDef* table_def) {
  assert(tuple != NULL);

  if (tuple->null_bitmap[idx]) {
    return MakeDatum(T_NULL, NULL);
  }

  DataLoc* locs = GetDataLocs(tuple);
  byte* data = GetDataPtr(tuple) + locs[idx].offset;
  Datum datum = MakeDatum(table_def->tuple_desc[idx].type, data);
  datum.length = locs[idx].length;
  return datum;
}

Tuple* SetCol(Tuple* tuple, const char* col_name, Datum datum, const TableDef* table_def) {
  bool is_missing = false;
  size_t change_idx = GetColIdx(table_def, col_name, &is_missing);
  assert(!is_missing);

  DataLoc* old_locs = GetDataLocs(tuple);
  byte* old_data_ptr = GetDataPtr(tuple);

  size_t new_size = tuple->length + datum.length - old_locs[change_idx].length;
  Tuple* new_tuple = calloc(new_size, sizeof(unsigned char));
  new_tuple->num_cols = tuple->num_cols;
  new_tuple->length = new_size;
  new_tuple->self_tid = tuple->self_tid;
  memcpy(new_tuple->null_bitmap, tuple->null_bitmap, new_tuple->num_cols * sizeof(byte));

  DataLoc* new_locs = GetDataLocs(new_tuple);
  byte* new_data_ptr = GetDataPtr(new_tuple);

  new_tuple->null_bitmap[change_idx] = datum.type == T_NULL;

  // TODO: Is this right? Don't have to alloc whole new thing if data is less than or equal
  // to datum length;
  size_t cur_offset = 0;
  for (size_t i = 0; i < new_tuple->num_cols; ++i) {
    if (i == change_idx) {
      size_t length = datum.length;
      new_locs[i].offset = cur_offset;
      new_locs[i].length = length;
      memcpy(new_data_ptr + cur_offset, datum.data, length);
    } else {
      size_t length = old_locs[i].length;
      new_locs[i].offset = cur_offset;
      new_locs[i].length = length;
      memcpy(new_data_ptr + cur_offset, old_data_ptr + old_locs[i].offset, length);
    }

    cur_offset += new_locs[i].length;
  }
  // free(tuple);
  return new_tuple;
}

Tuple* CopyTuple(Tuple* tuple) {
  Tuple* new_tuple = (Tuple*)calloc(tuple->length, sizeof(unsigned char));
  new_tuple = memcpy(new_tuple, tuple, tuple->length);
  return new_tuple;
}

void CreateTable(TableDef* table_def) {
  assert(table_def != NULL);
  table_def->index = arrlenu(TableDefs);
  arrpush(TableDefs, *table_def);
  arrpush(TablePages, NULL);
  RelStorageManager* sm = SMOpen(table_def->index);
  assert(sm != NULL);
  SMCreate(sm);
  // Make sure we add the table def to the system catalog.
  Tuple* table_def_tuple = SerializeTableDef(table_def);
  Cursor cursor;
  CursorInit(&cursor, &RelCatalogTableDef);
  CursorInsertTuple(&cursor, table_def_tuple);
}

void PageInit(Page page, uint16_t reserve_size) {
  assert(page != NULL);
  memset(page, 0, PAGE_SIZE);
  PageHeader* header = GetPageHeader(page);
  header->special_pointer_offset = PAGE_SIZE - reserve_size;
  header->free_lower_offset = sizeof(PageHeader);  // Is this off by one?
  header->free_upper_offset = header->special_pointer_offset - 1;
  header->num_locs = 0;
}

uint16_t PageGetFreeStart(Page page) {
  assert(page != NULL);
  PageHeader* header = GetPageHeader(page);
  return sizeof(header) + sizeof(ItemLoc) * header->num_locs;
}

bool PageAddItem(Page page, unsigned char* item, size_t size) {
  assert(page != NULL);
  assert(item != NULL);
  assert(size < PAGE_SIZE);
  PageHeader* header = GetPageHeader(page);

  // Should be a safe cast because we assume item fits on a page.
  uint16_t length = (uint16_t)size;
  // TODO: Off by one here?
  if (header->free_upper_offset - header->free_lower_offset - sizeof(ItemLoc) < length) {
    return false;
  }

  uint16_t offset = header->free_upper_offset - length + 1;
  ItemLoc loc;
  loc.offset = offset;
  loc.length = length;
  loc.dead = false;
  header->item_locs[header->num_locs] = loc;
  ++header->num_locs;
  memmove(page + offset, item, length);

  header->free_lower_offset += sizeof(ItemLoc);
  header->free_upper_offset = offset - 1;
  return true;
}

unsigned char* PageGetItem(Page page, size_t item_id) {
  assert(page != NULL);

  PageHeader* header = GetPageHeader(page);
  if (item_id >= header->num_locs) {
    return NULL;
  }
  ItemLoc loc = header->item_locs[item_id];
  return page + loc.offset;
}

uint16_t PageGetItemSize(Page page, size_t item_id) {
  assert(page != NULL);

  PageHeader* header = GetPageHeader(page);
  assert(item_id < header->num_locs);
  ItemLoc loc = header->item_locs[item_id];
  return loc.length;
}

void PageDeleteItem(Page page, size_t item_id) {
  assert(page != NULL);

  PageHeader* header = GetPageHeader(page);
  assert(item_id < header->num_locs);
  // Mark item loc for the tuple as being dead without removing it. We need to be careful
  // when removing item locs so that we update all tuple's tid reference.
  header->item_locs[item_id].dead = true;
  return;
}

void PageRemoveLoc(Page page, size_t item_id) {
  assert(page != NULL);

  PageHeader* header = GetPageHeader(page);
  if (item_id >= header->num_locs) {
    return;
  }
  ItemLoc loc_to_remove = PageGetItemLoc(page, item_id);
  // Shift item locs down by to cover the removed loc.
  for (uint16_t i = item_id; i < header->num_locs; ++i) {
    PageGetItemLoc(page, i) = PageGetItemLoc(page, i + 1);
  }
  --header->num_locs;
  // "grow" free space by moving it back since we've removed an item loc.
  header->free_lower_offset -= sizeof(ItemLoc);

  // Shift data the amount of the data item we removed.
  memmove(page + header->free_upper_offset + loc_to_remove.length,
          page + header->free_upper_offset, loc_to_remove.offset - header->free_upper_offset);
  // Grow free space on page by moving upper offset.
  header->free_upper_offset += loc_to_remove.length;
  // Since we shifted all of the data items, we need to shift/fix the itemloc pointers' offsets
  // by the amount we shifted the data items.
  for (uint16_t i = 0; i < header->num_locs; ++i) {
    ItemLoc* loc = &PageGetItemLoc(page, i);
    if (loc->offset <= loc_to_remove.offset) {
      loc->offset += loc_to_remove.length;
    }
  }
  return;
}

bool PageAddItemAt(Page page, size_t idx, unsigned char* item, size_t size) {
  assert(idx <= PageGetNumLocs(page));
  // Insert item if space.
  bool ok = PageAddItem(page, item, size);
  if (!ok) {
    return false;
  }

  // Move the item loc (which was appended to the end of the locs) to the right position.
  ItemLoc inserted_loc = PageGetItemLoc(page, PageGetNumLocs(page) - 1);
  for (uint16_t i = PageGetNumLocs(page) - 1; i > idx; --i) {
    PageGetItemLoc(page, i) = PageGetItemLoc(page, i - 1);
  }
  PageGetItemLoc(page, idx) = inserted_loc;
  return true;
}

void CursorInit(Cursor* cursor, TableDef* table_def) {
  assert(cursor != NULL);
  assert(table_def != NULL);
  cursor->table_index = table_def->index;
  cursor->page_index = 0;
  cursor->tuple_index = 0;
  cursor->rel_name = table_def->name;
}

Tuple* CursorSeekNext(Cursor* cursor) {
  assert(cursor != NULL);

  for (Page cur_page = ReadPage(cursor->table_index, cursor->page_index); cur_page != NULL;
       cur_page = ReadPage(cursor->table_index, cursor->page_index)) {
    if (cursor->tuple_index >= PageGetNumLocs(cur_page)) {
      ++cursor->page_index;
      cursor->tuple_index = 0;
      continue;
    }
    ItemLoc loc = PageGetItemLoc(cur_page, cursor->tuple_index);
    if (loc.dead) {
      ++cursor->tuple_index;
      continue;
    }

    unsigned char* tuple = PageGetItem(cur_page, cursor->tuple_index);
    assert(tuple != NULL);
    ++cursor->tuple_index;
    return (Tuple*)tuple;
  }

  return NULL;
}

void CursorInsertTuple(Cursor* cursor, Tuple* tuple) {
  assert(cursor != NULL);
  assert(tuple != NULL);
  Page cur_page = NULL;
  for (cur_page = ReadPage(cursor->table_index, cursor->page_index); cur_page != NULL;
       ++cursor->page_index, cur_page = ReadPage(cursor->table_index, cursor->page_index)) {
    uint16_t next_loc = GetPageNextLocNum(cur_page);
    TupleId tuple_id = {.page_num = cursor->page_index, .loc_num = next_loc};
    tuple->self_tid = tuple_id;
    if (PageAddItem(cur_page, (unsigned char*)tuple, TupleGetSize(tuple))) {
      WritePage(cursor->table_index, cursor->page_index, cur_page);
      return;
    }
  }
  TupleId tuple_id = {.page_num = cursor->page_index, .loc_num = 0};
  tuple->self_tid = tuple_id;
  cur_page = (Page)calloc(PAGE_SIZE, sizeof(byte));
  PageInit(cur_page, 0);
  assert(PageAddItem(cur_page, (unsigned char*)tuple, TupleGetSize(tuple)));
  WritePage(cursor->table_index, cursor->page_index, cur_page);
  return;
}

void CursorDeleteTupleById(Cursor* cursor, TupleId tid) {
  assert(cursor != NULL);

  Page page = ReadPage(cursor->table_index, tid.page_num);
  assert(page != NULL);

  assert(tid.loc_num < GetPageHeader(page)->num_locs);
  PageDeleteItem(page, tid.loc_num);
  WritePage(cursor->table_index, tid.page_num, page);
  return;
}

void CursorUpdateTupleById(Cursor* cursor, Tuple* updated_tuple, TupleId tid) {
  assert(cursor != NULL);
  assert(updated_tuple != NULL);

  Page page = ReadPage(cursor->table_index, tid.page_num);
  assert(tid.loc_num < GetPageHeader(page)->num_locs);

  PageDeleteItem(page, tid.loc_num);
  WritePage(cursor->table_index, tid.page_num, page);

  size_t page_index = tid.page_num;
  Page cur_page = ReadPage(cursor->table_index, page_index);
  for (; cur_page != NULL;
       ++page_index, cur_page = ReadPage(cursor->table_index, page_index)) {
    uint16_t next_loc = GetPageNextLocNum(cur_page);
    TupleId tuple_id = {.page_num = page_index, .loc_num = next_loc};
    updated_tuple->self_tid = tuple_id;
    if (PageAddItem(cur_page, (unsigned char*)updated_tuple, TupleGetSize(updated_tuple))) {
      WritePage(cursor->table_index, page_index, cur_page);
      return;
    }
  }
  TupleId tuple_id = {.page_num = page_index, .loc_num = 0};
  updated_tuple->self_tid = tuple_id;
  Page new_page = (Page)calloc(8192, sizeof(unsigned char));
  PageInit(new_page, 0);
  assert(PageAddItem(new_page, (unsigned char*)updated_tuple, TupleGetSize(updated_tuple)));
  WritePage(cursor->table_index, page_index, new_page);
}

Page ReadPage(uint64_t rel_id, PageId page_id) {
  Page page = (Page)calloc(PAGE_SIZE, sizeof(byte));
  assert(page != NULL);
  RelStorageManager* sm = SMOpen(rel_id);
  assert(sm != NULL);
  int result = SMRead(sm, page_id, page);
  if (result == 0) {
    return NULL;
  }
  assert(result == PAGE_SIZE);
  return page;
}

void WritePage(uint64_t rel_id, PageId page_id, Page page) {
  RelStorageManager* sm = SMOpen(rel_id);
  assert(sm != NULL);
  SMWrite(sm, page_id, page);
}

char* SMMakeRelPath(RelStorageManager* sm) {
  char* rel_path = calloc(sizeof("data_dir") + strlen(sm->rel_name) + 1, sizeof(char));
  rel_path = strcat(rel_path, "data_dir/");
  rel_path = strcat(rel_path, sm->rel_name);
  return rel_path;
}

RelStorageManager* SMOpen(uint64_t rel_id) {
  for (uint64_t i = 0; i < arrlenu(SMS); ++i) {
    if (SMS[i].rel_id == rel_id) {
      return &SMS[i];
    }
  }

  const TableDef* table_def = &TableDefs[rel_id];

  // Don't actually open any files.
  arrpush(SMS, ((RelStorageManager){
                   .fd = -1, .rel_id = rel_id, .rel_name = strdup(table_def->name)}));
  return &SMS[arrlenu(SMS) - 1];
}

void SMCreate(RelStorageManager* sm) {
  assert(sm != NULL);
  assert(sm->rel_name != NULL);
  assert(sm->fd == -1);
  char* rel_path = SMMakeRelPath(sm);
  int fd = open(rel_path, O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
  assert(fd != -1);
  sm->fd = fd;

  // Write first page so that there is something in the file. I think this will prevent
  // issues where we try to read the first page from an empty table.
  Page page = (Page)calloc(PAGE_SIZE, sizeof(byte));
  PageInit(page, 0);
  assert(page != NULL);
  SMWrite(sm, 0, page);
  return;
}

int SMRead(RelStorageManager* sm, PageId page_id, byte* buffer) {
  assert(sm != NULL);
  assert(sm->rel_name != NULL);
  assert(buffer != NULL);

  if (sm->fd == -1) {
    char* rel_path = SMMakeRelPath(sm);
    int fd = open(rel_path, O_RDWR);
    assert(fd != -1);
    sm->fd = fd;
  }

  // NOTE: maybe need to worry about overflow issues at some point?
  off_t seek_pos = page_id * PAGE_SIZE;
  int result = lseek(sm->fd, seek_pos, SEEK_SET);
  if (result != seek_pos) {
    Panic("lseek result != seek_pos");
  }

  return read(sm->fd, buffer, PAGE_SIZE);
}

void SMWrite(RelStorageManager* sm, PageId page_id, byte* buffer) {
  assert(sm != NULL);
  assert(sm->rel_name != NULL);
  assert(buffer != NULL);

  if (sm->fd == -1) {
    char* rel_path = SMMakeRelPath(sm);
    int fd = open(rel_path, O_RDWR);
    assert(fd != -1);
    sm->fd = fd;
  }

  // NOTE: maybe need to worry about overflow issues at some point?
  off_t seek_pos = page_id * PAGE_SIZE;
  int result = lseek(sm->fd, seek_pos, SEEK_SET);
  if (result != seek_pos) {
    Panic("lseek result != seek_pos");
  }

  result = write(sm->fd, buffer, PAGE_SIZE);
  assert(result == PAGE_SIZE);
  // Make sure stuff gets to disk (On Linux at least)!
  result = fsync(sm->fd);
  assert(result == 0);
  return;
}

PageId SMNumPages(RelStorageManager* sm) {
  assert(sm != NULL);

  if (sm->fd == -1) {
    char* rel_path = SMMakeRelPath(sm);
    int fd = open(rel_path, O_RDWR);
    assert(fd != -1);
    sm->fd = fd;
  }

  off_t file_length = lseek(sm->fd, 0, SEEK_END);
  assert(file_length >= 0);
  return file_length / PAGE_SIZE;
}

//////////////////////////////////////////////////////
// B-Tree Index Code.
//////////////////////////////////////////////////////

IndexDef* GetIndexDef(size_t idx) {
  if (idx >= arrlenu(IndexDefs)) {
    return NULL;
  }
  return &IndexDefs[idx];
}

IndexDef* CreateBTreeIndex(const TableDef* table_def, size_t* col_idxs) {
  assert(table_def != NULL);
  assert(col_idxs != NULL);

  // Index name is equal to "_".join([table_name, covering col names, index])
  size_t index_name_size = strlen(table_def->name) + strlen("_") + 1;
  for (size_t i = 0; i < arrlenu(col_idxs); ++i) {
    ColDesc desc = table_def->tuple_desc[col_idxs[i]];
    index_name_size += strlen(desc.column_name) + strlen("_");
  }
  index_name_size += strlen("index");

  char* index_name = (char*)calloc(index_name_size, sizeof(char));
  index_name = strcat(index_name, table_def->name);
  index_name = strcat(index_name, "_");
  for (size_t i = 0; i < arrlenu(col_idxs); ++i) {
    ColDesc desc = table_def->tuple_desc[col_idxs[i]];
    index_name = strcat(index_name, desc.column_name);
    index_name = strcat(index_name, "_");
  }
  index_name = strcat(index_name, "index");

  // Create table def for index tuples + create actual table that will hold the index.
  ColDesc* tuple_desc = NULL;
  for (size_t i = 0; i < arrlenu(col_idxs); ++i) {
    ColDesc col_desc;
    col_desc.column_name = strdup(table_def->tuple_desc[col_idxs[i]].column_name);
    col_desc.type = table_def->tuple_desc[col_idxs[i]].type;
    arrpush(tuple_desc, col_desc);
  }
  TableDef index_table_def;
  index_table_def.name = strdup(index_name);
  index_table_def.tuple_desc = tuple_desc;
  CreateTable(&index_table_def);

  // Create index definition for the index.
  IndexDef index_def = {.index_id = arrlenu(IndexDefs),
                        .index_name = index_name,
                        .col_idxs = col_idxs,
                        .table_def_idx = table_def->index,
                        .index_table_def_idx = index_table_def.index};
  arrpush(IndexDefs, index_def);

  Tuple* tuple = SerializeIndexDef(&index_def);
  assert(tuple != NULL);
  Cursor cursor;
  CursorInit(&cursor, &IndexCatalogTableDef);
  CursorInsertTuple(&cursor, tuple);

  // Init the metadata page for the index.
  Page page = calloc(PAGE_SIZE, sizeof(byte));
  BTreeMetaPageInit(page);
  WritePage(index_table_def.index, 0, page);
  return &IndexDefs[index_def.index_id];
}

Tuple* SerializeIndexDef(const IndexDef* index_def) {
  // Kind of dumb, but we store the col_idxs array serialized as a character string of numbers
  // separated by ",".
  size_t col_idxs_str_size = 1;
  for (size_t i = 0; i < arrlenu(index_def->col_idxs); ++i) {
    char str[256];
    int err = sprintf(str, "%zu", index_def->col_idxs[i]);
    if (err < 0) {
      Panic("Failed to convert size_t to str");
    }
    col_idxs_str_size += strlen(str);
    if (i < arrlenu(index_def->col_idxs) - 1) {
      col_idxs_str_size += strlen(",");
    }
  }
  char* col_idxs_str = (char*)calloc(col_idxs_str_size, sizeof(char));
  for (size_t i = 0; i < arrlenu(index_def->col_idxs); ++i) {
    char str[256];
    int err = sprintf(str, "%zu", index_def->col_idxs[i]);
    if (err < 0) {
      Panic("Failed to convert size_t to str");
    }
    col_idxs_str = strcat(col_idxs_str, str);
    if (i < arrlenu(index_def->col_idxs) - 1) {
      col_idxs_str = strcat(col_idxs_str, ",");
    }
  }

  int32_t* index_id = (int32_t*)calloc(1, sizeof(int32_t));
  *index_id = (int32_t)index_def->index_id;

  int32_t* table_def_idx = (int32_t*)calloc(1, sizeof(int32_t));
  *table_def_idx = (int32_t)index_def->table_def_idx;

  int32_t* index_table_def_idx = (int32_t*)calloc(1, sizeof(int32_t));
  *index_table_def_idx = (int32_t)index_def->index_table_def_idx;

  Tuple* tuple = MakeTuple(&IndexCatalogTableDef);
  tuple = SetCol(tuple, "index_id", MakeDatum(T_INT, index_id), &IndexCatalogTableDef);
  tuple = SetCol(tuple, "index_name", MakeDatum(T_STRING, strdup(index_def->index_name)),
                 &IndexCatalogTableDef);
  tuple = SetCol(tuple, "col_idxs", MakeDatum(T_STRING, col_idxs_str), &IndexCatalogTableDef);
  tuple =
      SetCol(tuple, "table_def_idx", MakeDatum(T_INT, table_def_idx), &IndexCatalogTableDef);
  tuple = SetCol(tuple, "index_table_def_idx", MakeDatum(T_INT, index_table_def_idx),
                 &IndexCatalogTableDef);
  return tuple;
}

void DeserializeIndexDef(Tuple* tuple, IndexDef* index_def) {
  size_t* col_idxs = NULL;
  char* save_ptr = NULL;
  char* tok = strtok_r(GetCol(tuple, "col_idxs", &IndexCatalogTableDef).data, ",", &save_ptr);
  assert(tok != NULL);
  while (tok != NULL) {
    size_t col_idx;
    int result = sscanf("%zu", tok, &col_idx);
    if (result == EOF) {
      Panic("Failed to convert str to size_t");
    }
    arrpush(col_idxs, col_idx);
    tok = strtok_r(NULL, ",", &save_ptr);
  }

  int32_t* index_id_ptr = (int32_t*)GetCol(tuple, "index_id", &IndexCatalogTableDef).data;
  int32_t* table_def_idx_ptr =
      (int32_t*)GetCol(tuple, "table_def_idx", &IndexCatalogTableDef).data;
  int32_t* index_table_def_idx_ptr =
      (int32_t*)GetCol(tuple, "index_table_def_idx", &IndexCatalogTableDef).data;

  index_def->index_id = (size_t)(*index_id_ptr);
  index_def->index_name = strdup(GetCol(tuple, "index_name", &IndexCatalogTableDef).data);
  index_def->col_idxs = col_idxs;
  index_def->table_def_idx = (size_t)(*table_def_idx_ptr);
  index_def->index_table_def_idx = (size_t)(*index_table_def_idx_ptr);
  return;
}

TableDef* IndexDefGetParentTableDef(const IndexDef* index_def) {
  assert(index_def != NULL);
  return &TableDefs[index_def->table_def_idx];
}

void BTreeMetaPageInit(Page page) {
  PageInit(page, sizeof(BTreeMetaPageInfo));
  BTreeMetaPageInfo* info = PageGetBTreeMetaPageInfo(page);
  info->root_page_id = NULL_PAGE;
  return;
}

IndexTuple* MakeIndexTuple(const IndexDef* index_def, Tuple* table_tuple) {
  assert(index_def != NULL);
  assert(table_tuple != NULL);
  const TableDef* parent_table_def = IndexDefGetParentTableDef(index_def);
  const TableDef* index_table_def = &TableDefs[index_def->index_table_def_idx];

  // Build a tuple that only contains the projections of the columns that are indexed.
  Tuple* index_tuple_data = MakeTuple(index_table_def);
  for (size_t i = 0; i < arrlenu(index_table_def->tuple_desc); ++i) {
    ColDesc desc = index_table_def->tuple_desc[i];
    index_tuple_data =
        SetCol(index_tuple_data, desc.column_name,
               GetCol(table_tuple, desc.column_name, parent_table_def), index_table_def);
  }

  // Copy the projected index tuple data into a buffer that is contiguous with an index tuple
  // header struct.
  IndexTuple* index_tuple =
      calloc(sizeof(IndexTuple) + TupleGetSize(index_tuple_data), sizeof(byte));
  memcpy(IndexTupleGetTuplePtr(index_tuple), index_tuple_data, TupleGetSize(index_tuple_data));
  // TODO: Need to think about this more. self_tid could point to a more updated tuple. Is this
  // okay?
  index_tuple->pointer = table_tuple->self_tid;
  return index_tuple;
}

void BTreePageSetHighKey(Page page, IndexTuple* new_high_key) {
  PageRemoveLoc(page, HIGH_KEY);
  PageAddItemAt(page, HIGH_KEY, (unsigned char*)new_high_key, IndexTupleGetSize(new_high_key));
  return;
}

void BTreePageInit(Page page, uint64_t level, uint16_t flags) {
  PageInit(page, sizeof(BTreePageInfo));
  BTreePageInfo* info = PageGetBTreePageInfo(page);
  info->level = level;
  info->flags = flags;

  // Init the high key to be empty.
  IndexTuple* high_key = (IndexTuple*)calloc(sizeof(IndexTuple) + sizeof(Tuple), sizeof(byte));
  Tuple* high_key_tuple = IndexTupleGetTuplePtr(high_key);
  high_key_tuple->length = sizeof(Tuple);
  PageAddItemAt(page, HIGH_KEY, (unsigned char*)high_key, IndexTupleGetSize(high_key));
}

CmpFunc TypeToCmpFunc(BType type) {
  switch (type) {
    case T_STRING: {
      return StrGT;
    }
    case T_INT: {
      return IntGT;
    }
    case T_BOOL: {
      return BoolGT;
    }
    case T_NULL:
    case T_UNKNOWN:
    default: {
      Panic("Unsopported type for btree index");
      return NULL;
    }
  }
}

uint16_t GetInsertionIdx(const IndexDef* index_def, SearchKey* sk, Page cur_page) {
  const TableDef* index_table_def = &TableDefs[index_def->index_table_def_idx];

  // Find index where item loc should be to be in sorted order.
  // Non-leaf pages treat the index tuple at location 0 as the low key.
  uint16_t i = BTreePageGetFirstKey(cur_page);
  for (; i < PageGetNumLocs(cur_page); ++i) {
    IndexTuple* cur_tuple = (IndexTuple*)PageGetItem(cur_page, i);
    // Datum d1 = GetColByIdx(IndexTupleGetTuplePtr(new_tuple), 0, parent_table_def);
    Datum d1 = sk->search_value;
    Datum d2 = GetColByIdx(IndexTupleGetTuplePtr(cur_tuple), 0, index_table_def);
    CmpFunc cmp_func = sk->cmp_func;
    // TODO: This needs to be a more more complicated comparison function.
    if (!GetBoolResult(cmp_func(d1, d2))) {
      return i;
    }
  }
  if (BTreePageIsRightMost(cur_page)) {
    // Right most pages, do not have a high key (since there is no right neighbor bounding what
    // keys the rightmost node could contain), so the insertion index should be at the end of
    // the items on the current page.
    return PageGetNumLocs(cur_page);
  }

  // Check high key.
  IndexTuple* cur_tuple = (IndexTuple*)PageGetItem(cur_page, HIGH_KEY);
  Datum d1 = sk->search_value;
  Datum d2 = GetColByIdx(IndexTupleGetTuplePtr(cur_tuple), 0, index_table_def);
  CmpFunc cmp_func = sk->cmp_func;
  // If the key we are trying to insert is greater than the high key of the page, we signal
  // that the insertion point is in a right neighbor by returning the index of the high key.
  // Otherwise, we need to insert at the end of the keys/before the high key.
  return GetBoolResult(cmp_func(d1, d2)) ? HIGH_KEY : PageGetNumLocs(cur_page);
}

void BTreeIndexInsert(const IndexDef* index_def, Tuple* table_tuple) {
  const TableDef* parent_table_def = &TableDefs[index_def->table_def_idx];
  IndexTuple* new_tuple = MakeIndexTuple(index_def, table_tuple);
  SearchKey new_tuple_sk;
  SearchKeyInit(&new_tuple_sk,
                GetColByIdx(IndexTupleGetTuplePtr(new_tuple), 0, parent_table_def));
  PageId root_id = BTreeReadOrCreateRootPageId(index_def);

  // Keep track of path through the tree in order to recursive back up and fix the tree after page
  // split inserts.
  PageId* path = NULL;
  PageId cur_page_id = root_id;
  Page cur_page = ReadPage(index_def->index_table_def_idx, cur_page_id);
  uint16_t i = GetInsertionIdx(index_def, &new_tuple_sk, cur_page);
  // Traverse index page list until we either find the correctly sorted position to place the
  // new element, or we get to the end of the page list.
  while (i == HIGH_KEY && BTreePageGetRight(cur_page) != NULL_PAGE) {
    // Move right.
    arrpush(path, cur_page_id);
    cur_page_id = BTreePageGetRight(cur_page);
    cur_page = ReadPage(index_def->index_table_def_idx, cur_page_id);
    i = GetInsertionIdx(index_def, &new_tuple_sk, cur_page);
  }

  while (true) {
    // TODO: move_right(). Not necessary now because we have no concurrent updates, so once we
    // get to this point, we are guaranteed to be at the leaf node where we can insert the new
    // tuple to be in the correct order (unless we need to split the node).

    // If there is enough space to insert the tuple, do so and reorder the keys.
    if (PageGetFreeSpace(cur_page) >= IndexTupleGetSize(new_tuple)) {
      uint16_t i = GetInsertionIdx(index_def, &new_tuple_sk, cur_page);

      bool ok =
          PageAddItemAt(cur_page, i, (unsigned char*)new_tuple, IndexTupleGetSize(new_tuple));
      assert(ok);
      WritePage(index_def->index_table_def_idx, cur_page_id, cur_page);
      return;
    } else {
      // We must split the node.
      BTreePageInfo* cur_page_info = PageGetBTreePageInfo(cur_page);

      // Extend index file.
      RelStorageManager* sm = SMOpen(index_def->index_table_def_idx);
      PageId num_pages = SMNumPages(sm);
      PageId new_page_id = num_pages;

      // Initialize new page.
      Page new_page = (Page)calloc(PAGE_SIZE, sizeof(byte));
      BTreePageInit(new_page, BTreePageGetLevel(cur_page), BTreePageIsLeaf(cur_page));
      BTreePageInfo* new_page_info = PageGetBTreePageInfo(new_page);
      new_page_info->right = cur_page_info->right;

      Page cur_page_new = (Page)calloc(PAGE_SIZE, sizeof(byte));
      BTreePageInit(cur_page_new, BTreePageGetLevel(cur_page), BTreePageIsLeaf(cur_page));
      BTreePageInfo* cur_page_new_info = PageGetBTreePageInfo(cur_page_new);
      cur_page_new_info->right = cur_page_info->right;

      // Because we have not inserted the new item yet, we need to get where we "would've"
      // inserted it. This is so that when we are calculating the split point, we make sure
      // to take into account the item to be inserted.
      uint16_t orig_insertion_idx = GetInsertionIdx(index_def, &new_tuple_sk, cur_page);

      // Pull out all of the locs for the current page. The locs have the tuple sizes which
      // we will use to calculate how much free space we open up by shifting them to the new
      // page.
      ItemLoc* locs = NULL;
      arrsetcap(locs, PageGetNumLocs(cur_page) + 1);
      for (uint16_t i = 0; i < PageGetNumLocs(cur_page); ++i) {
        arrpush(locs, PageGetItemLoc(cur_page, i));
      }

      // We need to account for the size of the new tuple, but we cannot insert it into a
      // page yet. So "virtually" insert it by creating the ItemLoc for it in order to
      // account for its size.
      ItemLoc new_tuple_loc = {
          .offset = 0, .length = IndexTupleGetSize(new_tuple), .dead = false};
      arrins(locs, orig_insertion_idx, new_tuple_loc);

      // Get the amount of free space left in the current page since we need to calculate how many
      // items to move over to the new page in order to free up enough space to insert the new item.
      // NOTE: PageGetFreeSpace subtracts an item loc, so we add back here for transparency.
      ssize_t left_page_space_free = PageGetFreeSpace(cur_page) + sizeof(ItemLoc);
      // Since we are going to change the high key on the left page, make sure we add back
      // the space.
      left_page_space_free += PageGetItemSize(cur_page, HIGH_KEY) + sizeof(ItemLoc);
      // Since we've virtually added the new index tuple to the left page, subtract the size of the tuple. 
      left_page_space_free -= IndexTupleGetSize(new_tuple) + sizeof(ItemLoc);

      // Set the high key of the right page to be the current high key of the left page. Do this early
      // so that we take into account the right page high key size when calculating split index.
      // TODO: Possibly needs more testing.
      IndexTuple* cur_page_high_key =
          (IndexTuple*)PageGetItem(cur_page, BTreePageGetFirstKey(cur_page));
      PageRemoveLoc(new_page, HIGH_KEY);
      bool ok = PageAddItemAt(new_page, HIGH_KEY, (unsigned char*)cur_page_high_key,
                         IndexTupleGetSize(cur_page_high_key));
      assert(ok);
      // Grab amount of free space on new page, add size of ItemLoc to account for PageGetFreeSpace subtracting.
      ssize_t right_page_space_free = PageGetFreeSpace(new_page) + sizeof(ItemLoc);


      // Calculate the split index location. Starting from the right of the locs list, "move" an item to the right page,
      // add back free space to left, subtract moved item size from right, and check if the remaining free space of both
      // pages is >= 0. If it is, then we know we can split at this index and have enough space to add the new tuple after
      // moving items. split_idx is the index at which to split the items between the current page and new page in order
      // to make space for insertion of the new tuple. split_idx + 1 is the first item to move to the next page from the current page.
      uint16_t split_idx = arrlenu(locs)-1;
      ssize_t new_high_key_size = locs[split_idx].length + sizeof(IndexTuple) + sizeof(ItemLoc);
      for (; split_idx >= BTreePageGetFirstKey(cur_page);) {
        ItemLoc *loc = &locs[split_idx];
        ssize_t tuple_to_move_size = loc->length + sizeof(ItemLoc); // loc->length should be the full size of the tuple item.
        left_page_space_free += tuple_to_move_size;  // Add space to left side since we are removing the item.
        assert(right_page_space_free >= tuple_to_move_size); // Check for underflow.
        right_page_space_free -= tuple_to_move_size; // Subtract space from right side since we are adding the item.
        new_high_key_size = tuple_to_move_size;
        --split_idx;

        // Need to account for the new left page high key size.
        if (left_page_space_free - new_high_key_size >= 0 && right_page_space_free >= 0) {
          break;
        }
      }
      // If this happens, we did not find a valid split point, so just fail.
      assert(split_idx != BTreePageGetFirstKey(cur_page) - 1);

      // Because we virtually inserted the new index tuple into the locs list, we potentially need to adjust the actual
      // split index. If the virtually inserted tuple is before the split index, we need to shift the split index
      // left by 1.
      if (orig_insertion_idx <= split_idx) {
        --split_idx;
      }

      // Move items on left page to new copy (essentially deleting the items we moved to
      // the right page, just not in place).
      for (uint16_t j = BTreePageGetFirstKey(cur_page); j <= split_idx; ++j) {
        IndexTuple* t = (IndexTuple*)PageGetItem(cur_page, j);
        PageAddItem(cur_page_new, (unsigned char*)t, IndexTupleGetSize(t));
      }
      // Move items from current page over to new page.
      for (uint16_t j = split_idx + 1; j < PageGetNumLocs(cur_page); ++j) {
        IndexTuple* t = (IndexTuple*)PageGetItem(cur_page, j);
        PageAddItem(new_page, (unsigned char*)t, IndexTupleGetSize(t));
      }

      // Based on the original insertion idx of the new item and the split point, calculate
      // which page we should insert the new item into, get the new insertion point on that
      // page, and then insert the new item.
      // TODO: Possibly needs more testing.
      Page insert_page = orig_insertion_idx > split_idx ? new_page : cur_page_new;
      uint16_t insert_idx = GetInsertionIdx(index_def, &new_tuple_sk, insert_page);
      ok = PageAddItemAt(insert_page, insert_idx, (unsigned char*)new_tuple,
                              IndexTupleGetSize(new_tuple));
      assert(ok);

      // Set high key of cur_page to be the first key of new page.
      IndexTuple* new_cur_page_high_key =
          (IndexTuple*)PageGetItem(new_page, BTreePageGetFirstKey(new_page));
      PageRemoveLoc(cur_page_new, HIGH_KEY);
      ok = PageAddItemAt(cur_page_new, HIGH_KEY, (unsigned char*)new_cur_page_high_key,
                         IndexTupleGetSize(new_cur_page_high_key));
      assert(ok);

      // TODO: Set high key of new page to be old high key of cur_page.

      // Link old page to new page. Do this down here because some functions (like
      // calulating insert indexes on pages) depend on knowing if the page is the rightmost
      // page or not.
      cur_page_new_info->right = new_page_id;

      // Write out the modified pages.
      WritePage(index_def->index_table_def_idx, cur_page_id, cur_page_new);
      WritePage(index_def->index_table_def_idx, new_page_id, new_page);

      // ASSUMPTION: Assuming we always split root. This is just to get things working at first.
      // {
      //   PageId num_pages = SMNumPages(sm);
      //   PageId new_root_page_id = num_pages;

      //   // Initialize new root page.
      //   Page new_root_page = (Page)calloc(PAGE_SIZE, sizeof(byte));
      //   BTreePageInit(new_root_page, BTreePageGetLevel(cur_page) + 1, false);
      //   BTreePageInfo* new_root_page_info = PageGetBTreePageInfo(new_root_page);
      //   cur_page_id = new_root_page_id;
      //   cur_page = new_root_page;
      // }

      return;
    }
  }
}

Page BTreeReadMetaPage(const IndexDef* index_def) {
  return ReadPage(index_def->index_table_def_idx, 0);
}

PageId BTreeReadOrCreateRootPageId(const IndexDef* index_def) {
  Page meta_page = BTreeReadMetaPage(index_def);
  BTreeMetaPageInfo* meta_info = PageGetBTreeMetaPageInfo(meta_page);
  if (meta_info->root_page_id != NULL_PAGE) {
    return meta_info->root_page_id;
  }
  // If root page is not initialized (i.e. the tree is empty), create it.
  Page root_page = (Page)calloc(PAGE_SIZE, sizeof(byte));
  BTreePageInit(root_page, 0, ROOT_PAGE | LEAF_PAGE);
  WritePage(index_def->index_table_def_idx, 1, root_page);
  // Make sure we update the meta page to point to the new root.
  meta_info->root_page_id = 1;
  WritePage(index_def->index_table_def_idx, 0, meta_page);
  return 1;
}

void IndexCursorInit(IndexCursor* cursor, const IndexDef* index_def,
                     Datum boundry_search_key_value) {
  cursor->page_id = 0;
  cursor->tuple_id = 0;
  cursor->index_def = index_def;
  cursor->search_key = (SearchKey*)calloc(1, sizeof(SearchKey));
  SearchKeyInit(cursor->search_key, boundry_search_key_value);
}

Tuple* BTreeFirst(IndexCursor* cursor) {
  assert(cursor != NULL);

  cursor->page_id = BTreeReadOrCreateRootPageId(cursor->index_def);
  Page cur_page = ReadPage(cursor->index_def->index_table_def_idx, cursor->page_id);
  assert(cur_page != NULL);
  cursor->tuple_id = GetInsertionIdx(cursor->index_def, cursor->search_key, cur_page);
  if (cursor->tuple_id == HIGH_KEY || cursor->tuple_id == PageGetNumLocs(cur_page)) {
    // TODO: Handle traversing to right page.
    return NULL;
  }
  IndexTuple* index_tuple = (IndexTuple*)PageGetItem(cur_page, cursor->tuple_id);
  if (index_tuple == NULL) {
    return NULL;
  }
  Page table_page = ReadPage(cursor->index_def->table_def_idx, index_tuple->pointer.page_num);
  return (Tuple*)PageGetItem(table_page, index_tuple->pointer.loc_num);
}

Tuple* BTreeGetNext(IndexCursor* cursor, ScanDirection dir) {
  assert(cursor != NULL);
  assert(cursor->index_def != NULL);

  // TODO: Make sure this works.
  if (cursor->page_id == NULL_PAGE) {
    return NULL;
  }

  Page cur_page = ReadPage(cursor->index_def->index_table_def_idx, cursor->page_id);
  switch (dir) {
    case SCAN_FORWARD: {
      ++cursor->tuple_id;
      if (cursor->tuple_id >= PageGetNumLocs(cur_page)) {
        // TODO: Handle traversing right.
        cursor->page_id = BTreePageGetRight(cur_page);
        cur_page = ReadPage(cursor->index_def->index_table_def_idx, cursor->page_id);
        cursor->tuple_id = BTreePageGetFirstKey(cur_page);
      }
      break;
    }
    case SCAN_BACKWARDS: {
      --cursor->tuple_id;
      if (cursor->tuple_id <= BTreePageGetFirstKey(cur_page)) {
        // TODO: Handle traversing left.
        return NULL;
      }
      break;
    }
    default: {
      Panic("Unknown BTree Scan Direction");
    }
  }

  IndexTuple* index_tuple = (IndexTuple*)PageGetItem(cur_page, cursor->tuple_id);
  if (index_tuple == NULL) {
    return NULL;
  }
  // Grab table page item from index tuple.
  // TODO: handle dead tuples?
  Page table_page = ReadPage(cursor->index_def->table_def_idx, index_tuple->pointer.page_num);
  return (Tuple*)PageGetItem(table_page, index_tuple->pointer.loc_num);
}

void SearchKeyInit(SearchKey* sk, Datum search_value) {
  sk->search_value = search_value;
  sk->cmp_func = TypeToCmpFunc(search_value.type);
}
