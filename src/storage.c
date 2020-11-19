#include "storage.h"

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

#include "stb_ds.h"
#include "utils.h"

TableDef* TableDefs = NULL;
Page** TablePages = NULL;  // 2d stb array.
RelStorageManager* SMS = NULL;

Tuple* MakeTuple(TableDef* table_def) {
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
  ColDesc* tuple_desc = NULL;
  ColDesc col1 = {.column_name = "bar", .type = T_STRING};
  ColDesc col2 = {.column_name = "baz", .type = T_BOOL};
  arrpush(tuple_desc, col1);
  arrpush(tuple_desc, col2);
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
  ColDesc table2_col1 = {.column_name = "a", .type = T_STRING};
  arrpush(table2_tuple_desc, table2_col1);
  TableDef table_def2 = {.name = "b", .tuple_desc = table2_tuple_desc};
  CreateTable(&table_def2);

  CursorInit(&cursor, &table_def2);

  Tuple* table2_t1 = MakeTuple(&table_def2);
  table2_t1 = SetCol(table2_t1, "a", MakeDatum(T_STRING, strdup("asdf")), &table_def2);
  CursorInsertTuple(&cursor, table2_t1);
  Tuple* table2_t2 = MakeTuple(&table_def2);
  table2_t2 = SetCol(table2_t2, "a", MakeDatum(T_STRING, strdup("cab")), &table_def2);
  CursorInsertTuple(&cursor, table2_t2);
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

size_t GetColIdx(Tuple* tuple, const char* col_name, TableDef* table_def, bool* is_missing) {
  assert(tuple != NULL);
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

Datum GetCol(Tuple* tuple, const char* col_name, TableDef* table_def) {
  assert(tuple != NULL);
  assert(table_def != NULL);
  assert(table_def->tuple_desc != NULL);

  bool is_missing = false;
  size_t i = GetColIdx(tuple, col_name, table_def, &is_missing);
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

Tuple* SetCol(Tuple* tuple, const char* col_name, Datum datum, TableDef* table_def) {
  bool is_missing = false;
  size_t change_idx = GetColIdx(tuple, col_name, table_def, &is_missing);
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

  // TODO: Is this right? Don't have to alloc whole new thing if data is less than or equal to
  // datum length;
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
}

void PageInit(Page page) {
  assert(page != NULL);
  memset(page, 0, PAGE_SIZE);
  PageHeader* header = GetPageHeader(page);
  header->free_lower_offset = sizeof(PageHeader);  // Is this off by one?
  header->free_upper_offset = PAGE_SIZE - 1;
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
  // Mark item loc for the tuple as being dead without removing it. We need to be careful when
  // removing item locs so that we update all tuple's tid reference.
  header->item_locs[item_id].dead = true;
  return;
}

void CursorInit(Cursor* cursor, TableDef* table_def) {
  assert(cursor != NULL);
  assert(table_def != NULL);
  cursor->table_index = table_def->index;
  cursor->page_index = 0;
  cursor->tuple_index = 0;
}

Tuple* CursorSeekNext(Cursor* cursor) {
  assert(cursor != NULL);

  for (Page cur_page = ReadPage(cursor->table_index, NULL, cursor->page_index);
       cur_page != NULL; cur_page = ReadPage(cursor->table_index, NULL, cursor->page_index)) {
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
    // if (tuple == NULL) {
    //   ++cursor->page_index;
    //   cursor->tuple_index = 0;
    // } else {
    //   ++cursor->tuple_index;
    // }
    return (Tuple*)tuple;
  }

  return NULL;
}

// Tuple* CursorPeek(Cursor* cursor) {
//   assert(cursor != NULL);

//   // NOTE: Need to do this check or else I could run off the end of the dynamic array.
//   // arrdel() doesn't automatically clear the moved over spaces.
//   if (cursor->page_index >= arrlenu(TablePages[cursor->table_index])) {
//     return NULL;
//   }
//   return (Tuple*)PageGetItem(TablePages[cursor->table_index][cursor->page_index],
//                              cursor->tuple_index);
// }

void CursorInsertTuple(Cursor* cursor, Tuple* tuple) {
  assert(cursor != NULL);
  assert(tuple != NULL);
  Page cur_page = NULL;
  for (cur_page = ReadPage(cursor->table_index, NULL, cursor->page_index); cur_page != NULL;
       ++cursor->page_index,
      cur_page = ReadPage(cursor->table_index, NULL, cursor->page_index)) {
    uint16_t next_loc = GetPageNextLocNum(cur_page);
    TupleId tuple_id = {.page_num = cursor->page_index, .loc_num = next_loc};
    tuple->self_tid = tuple_id;
    if (PageAddItem(cur_page, (unsigned char*)tuple, TupleGetSize(tuple))) {
      return;
    }
  }
  TupleId tuple_id = {.page_num = cursor->page_index, .loc_num = 0};
  tuple->self_tid = tuple_id;
  cur_page = (Page)calloc(8192, sizeof(unsigned char));
  PageInit(cur_page);
  assert(PageAddItem(cur_page, (unsigned char*)tuple, TupleGetSize(tuple)));
  WritePage(cursor->table_index, NULL, cursor->page_index, cur_page);
  return;
}

void CursorDeleteTupleById(Cursor* cursor, TupleId tid) {
  assert(cursor != NULL);

  Page page = ReadPage(cursor->table_index, NULL, tid.page_num);
  assert(page != NULL);

  PageHeader* header = GetPageHeader(page);
  assert(tid.loc_num < header->num_locs);
  PageDeleteItem(page, tid.loc_num);
  return;
}

void CursorUpdateTupleById(Cursor* cursor, Tuple* updated_tuple, TupleId tid) {
  assert(cursor != NULL);
  assert(updated_tuple != NULL);
  // assert(tid.page_num < arrlenu(TablePages[cursor->table_index]));

  Page page = ReadPage(cursor->table_index, NULL, tid.page_num);
  PageHeader* header = GetPageHeader(page);
  assert(tid.loc_num < header->num_locs);

  PageDeleteItem(page, tid.loc_num);

  size_t page_index = tid.page_num;
  Page cur_page = ReadPage(cursor->table_index, NULL, page_index);
  for (; cur_page != NULL;
       ++page_index, cur_page = ReadPage(cursor->table_index, NULL, page_index)) {
    uint16_t next_loc = GetPageNextLocNum(cur_page);
    TupleId tuple_id = {.page_num = page_index, .loc_num = next_loc};
    updated_tuple->self_tid = tuple_id;
    if (PageAddItem(cur_page, (unsigned char*)updated_tuple, TupleGetSize(updated_tuple))) {
      return;
    }
  }
  TupleId tuple_id = {.page_num = page_index, .loc_num = 0};
  updated_tuple->self_tid = tuple_id;
  Page new_page = (Page)calloc(8192, sizeof(unsigned char));
  PageInit(new_page);
  assert(PageAddItem(new_page, (unsigned char*)updated_tuple, TupleGetSize(updated_tuple)));
  WritePage(cursor->table_index, NULL, page_index, new_page);
}

Page ReadPage(uint64_t rel_id, char* rel_name, uint64_t page_id) {
  assert(rel_name == NULL);
  if (page_id >= arrlenu(TablePages[rel_id])) {
    return NULL;
  }
  return TablePages[rel_id][page_id];
  // Page page = (Page)calloc(PAGE_SIZE, sizeof(byte));
  // assert(page != NULL);
  // RelStorageManager* sm = SMOpen(rel_id, rel_name);
  // assert(sm != NULL);
  // SMRead(sm, page_id, page);
  // return page;
}

void WritePage(uint64_t rel_id, char* rel_name, uint64_t page_id, Page page) {
  assert(rel_name == NULL);
  assert(page_id != 100000);
  arrpush(TablePages[rel_id], page);
  // assert(page != NULL);
  // RelStorageManager* sm = SMOpen(rel_id, rel_name);
  // assert(sm != NULL);
  // SMWrite(sm, page_id, page);
}

RelStorageManager* SMOpen(uint64_t rel_id, char* rel_name) {
  for (uint64_t i = 0; i < arrlenu(SMS); ++i) {
    if (SMS[i].rel_id == rel_id) {
      return &SMS[i];
    }
  }

  // Don't actually open any files.
  arrpush(SMS, ((RelStorageManager){.fd = -1, .rel_id = 0, .rel_name = strdup(rel_name)}));
  return &SMS[arrlenu(SMS) - 1];
}

void SMCreate(RelStorageManager* sm) {
  assert(sm != NULL);
  assert(sm->rel_name != NULL);
  assert(sm->fd == -1);
  char* rel_path = strcat("data_dir/", sm->rel_name);
  int fd = open(rel_path, O_RDWR | O_CREAT | O_EXCL);
  assert(fd != -1);
  sm->fd = fd;
  return;
}

void SMRead(RelStorageManager* sm, uint64_t page_id, byte* buffer) {
  assert(sm != NULL);
  assert(sm->rel_name != NULL);
  assert(buffer != NULL);

  if (sm->fd == -1) {
    char* rel_path = strcat("data_dir/", sm->rel_name);
    int fd = open(rel_path, O_RDWR);
    assert(fd != -1);
    sm->fd = fd;
  }

  // NOTE: maybe need to worry about overflow issues at some point?
  off_t seek_pos = page_id * PAGE_SIZE;
  int result = lseek(sm->fd, seek_pos, SEEK_SET);
  assert(result == seek_pos);

  result = read(sm->fd, buffer, PAGE_SIZE);
  assert(result == PAGE_SIZE);
  return;
}

void SMWrite(RelStorageManager* sm, uint64_t page_id, byte* buffer) {
  assert(sm != NULL);
  assert(sm->rel_name != NULL);
  assert(buffer != NULL);

  if (sm->fd == -1) {
    char* rel_path = strcat("data_dir/", sm->rel_name);
    int fd = open(rel_path, O_RDWR);
    assert(fd != -1);
    sm->fd = fd;
  }

  // NOTE: maybe need to worry about overflow issues at some point?
  off_t seek_pos = page_id * PAGE_SIZE;
  int result = lseek(sm->fd, seek_pos, SEEK_SET);
  assert(result == seek_pos);

  result = write(sm->fd, buffer, PAGE_SIZE);
  assert(result == PAGE_SIZE);
  // Make sure stuff gets to disk (On Linux at least)!
  result = fsync(sm->fd);
  assert(result == 0);
  return;
}
