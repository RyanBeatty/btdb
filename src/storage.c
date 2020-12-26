#include "storage.h"

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

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

    Cursor cursor;
    CursorInit(&cursor, &RelCatalogTableDef);

    // Materialize table defs into memory. A lot of code assumes that all table defs are
    // already materialized. Additionally, we assume that the relcatalog table stores table
    // defs in sorted index/creation order.
    Tuple* tuple = CursorSeekNext(&cursor);
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
  RelStorageManager* sm = SMOpen(table_def->index, table_def->name);
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

  for (Page cur_page = ReadPage(cursor->table_index, cursor->rel_name, cursor->page_index);
       cur_page != NULL;
       cur_page = ReadPage(cursor->table_index, cursor->rel_name, cursor->page_index)) {
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
  for (cur_page = ReadPage(cursor->table_index, cursor->rel_name, cursor->page_index);
       cur_page != NULL; ++cursor->page_index,
      cur_page = ReadPage(cursor->table_index, cursor->rel_name, cursor->page_index)) {
    uint16_t next_loc = GetPageNextLocNum(cur_page);
    TupleId tuple_id = {.page_num = cursor->page_index, .loc_num = next_loc};
    tuple->self_tid = tuple_id;
    if (PageAddItem(cur_page, (unsigned char*)tuple, TupleGetSize(tuple))) {
      WritePage(cursor->table_index, cursor->rel_name, cursor->page_index, cur_page);
      return;
    }
  }
  TupleId tuple_id = {.page_num = cursor->page_index, .loc_num = 0};
  tuple->self_tid = tuple_id;
  cur_page = (Page)calloc(8192, sizeof(unsigned char));
  PageInit(cur_page, 0);
  assert(PageAddItem(cur_page, (unsigned char*)tuple, TupleGetSize(tuple)));
  WritePage(cursor->table_index, cursor->rel_name, cursor->page_index, cur_page);
  return;
}

void CursorDeleteTupleById(Cursor* cursor, TupleId tid) {
  assert(cursor != NULL);

  Page page = ReadPage(cursor->table_index, cursor->rel_name, tid.page_num);
  assert(page != NULL);

  assert(tid.loc_num < GetPageHeader(page)->num_locs);
  PageDeleteItem(page, tid.loc_num);
  WritePage(cursor->table_index, cursor->rel_name, tid.page_num, page);
  return;
}

void CursorUpdateTupleById(Cursor* cursor, Tuple* updated_tuple, TupleId tid) {
  assert(cursor != NULL);
  assert(updated_tuple != NULL);

  Page page = ReadPage(cursor->table_index, cursor->rel_name, tid.page_num);
  assert(tid.loc_num < GetPageHeader(page)->num_locs);

  PageDeleteItem(page, tid.loc_num);
  WritePage(cursor->table_index, cursor->rel_name, tid.page_num, page);

  size_t page_index = tid.page_num;
  Page cur_page = ReadPage(cursor->table_index, cursor->rel_name, page_index);
  for (; cur_page != NULL;
       ++page_index, cur_page = ReadPage(cursor->table_index, cursor->rel_name, page_index)) {
    uint16_t next_loc = GetPageNextLocNum(cur_page);
    TupleId tuple_id = {.page_num = page_index, .loc_num = next_loc};
    updated_tuple->self_tid = tuple_id;
    if (PageAddItem(cur_page, (unsigned char*)updated_tuple, TupleGetSize(updated_tuple))) {
      WritePage(cursor->table_index, cursor->rel_name, page_index, cur_page);
      return;
    }
  }
  TupleId tuple_id = {.page_num = page_index, .loc_num = 0};
  updated_tuple->self_tid = tuple_id;
  Page new_page = (Page)calloc(8192, sizeof(unsigned char));
  PageInit(new_page, 0);
  assert(PageAddItem(new_page, (unsigned char*)updated_tuple, TupleGetSize(updated_tuple)));
  WritePage(cursor->table_index, cursor->rel_name, page_index, new_page);
}

Page ReadPage(uint64_t rel_id, const char* rel_name, PageId page_id) {
  assert(rel_name != NULL);
  Page page = (Page)calloc(PAGE_SIZE, sizeof(byte));
  assert(page != NULL);
  RelStorageManager* sm = SMOpen(rel_id, rel_name);
  assert(sm != NULL);
  int result = SMRead(sm, page_id, page);
  if (result == 0) {
    return NULL;
  }
  assert(result == PAGE_SIZE);
  return page;
}

void WritePage(uint64_t rel_id, const char* rel_name, PageId page_id, Page page) {
  assert(rel_name != NULL);
  RelStorageManager* sm = SMOpen(rel_id, rel_name);
  assert(sm != NULL);
  SMWrite(sm, page_id, page);
}

RelStorageManager* SMOpen(uint64_t rel_id, const char* rel_name) {
  for (uint64_t i = 0; i < arrlenu(SMS); ++i) {
    if (SMS[i].rel_id == rel_id) {
      return &SMS[i];
    }
  }

  // Don't actually open any files.
  arrpush(SMS,
          ((RelStorageManager){.fd = -1, .rel_id = rel_id, .rel_name = strdup(rel_name)}));
  return &SMS[arrlenu(SMS) - 1];
}

void SMCreate(RelStorageManager* sm) {
  assert(sm != NULL);
  assert(sm->rel_name != NULL);
  assert(sm->fd == -1);
  char* rel_path = calloc(sizeof("data_dir") + strlen(sm->rel_name) + 1, sizeof(char));
  rel_path = strcat(rel_path, "data_dir/");
  rel_path = strcat(rel_path, sm->rel_name);
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
    char* rel_path = calloc(sizeof("data_dir") + strlen(sm->rel_name) + 1, sizeof(char));
    rel_path = strcat(rel_path, "data_dir/");
    rel_path = strcat(rel_path, sm->rel_name);
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
    char* rel_path = calloc(sizeof("data_dir") + strlen(sm->rel_name) + 1, sizeof(char));
    rel_path = strcat(rel_path, "data_dir/");
    rel_path = strcat(rel_path, sm->rel_name);
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

//////////////////////////////////////////////////////
// B-Tree Index Code.
//////////////////////////////////////////////////////

void CreateBTreeIndex(const TableDef* table_def, size_t* col_idxs) {
  assert(table_def != NULL);
  assert(col_idxs != NULL);

  // Index name is equal to "_".join([table_name, covering col names, index])
  char* index_name = NULL;
  size_t index_name_size = strlen(table_def->name) + strlen("_") + 1;
  index_name = realloc(index_name, index_name_size);
  index_name = strcat(index_name, table_def->name);
  index_name = strcat(index_name, "_");
  for (size_t i = 0; i < arrlenu(col_idxs); ++i) {
    ColDesc desc = table_def->tuple_desc[col_idxs[i]];
    index_name_size += strlen(desc.column_name) + strlen("_");
    index_name = realloc(index_name, index_name_size);
    index_name = strcat(index_name, desc.column_name);
    index_name = strcat(index_name, "_");
  }
  index_name_size += strlen("index");
  index_name = realloc(index_name, index_name_size);
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
  WritePage(index_table_def.index, index_table_def.name, 0, page);
}

Tuple* SerializeIndexDef(const IndexDef* index_def) {
  // Kind of dumb, but we store the col_idxs array serialized as a character string of numbers
  // separated by ",".
  char* col_idxs_str = NULL;
  size_t col_idxs_str_size = 1;
  for (size_t i = 0; i < arrlenu(index_def->col_idxs); ++i) {
    char str[256];
    int err = sprintf(str, "%zu", index_def->col_idxs[i]);
    if (err < 0) {
      Panic("Failed to convert size_t to str");
    }
    col_idxs_str_size += strlen(str);
    col_idxs_str = realloc(col_idxs_str, col_idxs_str_size);
    col_idxs_str = strcat(col_idxs_str, str);
    if (i < arrlenu(index_def->col_idxs) - 1) {
      col_idxs_str_size += strlen(",");
      col_idxs_str = realloc(col_idxs_str, col_idxs_str_size);
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

void BTreeMetaPageInit(Page page) {
  PageInit(page, sizeof(BTreeMetaPageInfo));
  BTreeMetaPageInfo* info = PageGetBTreeMetaPageInfo(page);
  info->root_page_id = NULL_PAGE;
  return;
}

IndexTuple* MakeIndexTuple(const IndexDef* index_def, Tuple* table_tuple) {
  assert(index_def != NULL);
  assert(table_tuple != NULL);
  const TableDef* parent_table_def = &TableDefs[index_def->table_def_idx];
  const TableDef* index_table_def = &TableDefs[index_def->index_table_def_idx];

  // Build a tuple that only contains the projections of the columns that are indexed.
  Tuple* index_tuple_data = NULL;
  for (size_t i = 0; i < arrlenu(index_table_def->tuple_desc); ++i) {
    ColDesc desc = index_table_def->tuple_desc[i];
    index_tuple_data =
        SetCol(index_tuple_data, desc.column_name,
               GetCol(table_tuple, desc.column_name, parent_table_def), index_table_def);
  }

  // Copy the projected index tuple data into a buffer that is contiguous with an index tuple
  // header struct.
  IndexTuple* index_tuple =
      calloc(sizeof(IndexTuple) + TupleGetSize(table_tuple), sizeof(byte));
  memcpy(IndexTupleGetTuplePtr(index_tuple), table_tuple, table_tuple->length);
  return index_tuple;
}

void BTreePageInit(Page page, uint64_t level) {
  PageInit(page, sizeof(BTreePageInfo));
  BTreePageInfo* info = PageGetBTreePageInfo(page);
  info->level = level;
}

void BTreeIndexInsert(const IndexDef* index_def, Tuple* table_tuple) {
  IndexTuple* index_tuple = MakeIndexTuple(index_def, table_tuple);
  Page meta_page = ReadPage(index_def->index_table_def_idx,
                            TableDefs[index_def->index_table_def_idx].name, 0);
  BTreeMetaPageInfo* meta_info = PageGetBTreeMetaPageInfo(meta_page);
  PageId root_id = meta_info->root_page_id;
  if (root_id == NULL_PAGE) {
    // If root page is not initialized (i.e. the tree is empty), create it.
    Page root_page = (Page)calloc(PAGE_SIZE, sizeof(byte));
    BTreePageInit(root_page, 1);
    WritePage(index_def->index_table_def_idx, TableDefs[index_def->index_table_def_idx].name,
              1, root_page);
    // Make sure we update the meta page to point to the new root.
    meta_info->root_page_id = 1;
    WritePage(index_def->index_table_def_idx, TableDefs[index_def->index_table_def_idx].name,
              0, meta_page);
    root_id = 1;
  }
  Page root_page = ReadPage(index_def->index_table_def_idx,
                            TableDefs[index_def->index_table_def_idx].name, root_id);
  bool ok =
      PageAddItem(root_page, (unsigned char*)index_tuple, IndexTupleGetSize(index_tuple));
  // TODO: At the moment since we are still implementing btree indexes, we assume the root page
  // always has room for items. Fix this later.
  assert(ok);
  WritePage(index_def->index_table_def_idx, TableDefs[index_def->index_table_def_idx].name,
            root_id, root_page);
  return;
}
