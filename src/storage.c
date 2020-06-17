#include "storage.h"

#include <assert.h>
#include <stdbool.h>

#include "stb_ds.h"

TableDef* TableDefs = NULL;
Table* Tables = NULL;

Tuple* MakeTuple(TableDef* table_def) {
  assert(table_def != NULL);
  assert(table_def->tuple_desc != NULL);
  size_t num_cols = arrlen(table_def->tuple_desc);
  Tuple* t = calloc(1, sizeof(Tuple) + num_cols * sizeof(DataLoc) + num_cols * sizeof(byte));
  t->length = sizeof(Tuple) + num_cols * sizeof(DataLoc) + num_cols * sizeof(byte);
  t->num_cols = num_cols;
  t->data_offset = sizeof(Tuple) + num_cols * sizeof(DataLoc);
  byte* data_ptr = (byte*)t + t->data_offset;
  size_t cur_offset = 0;
  for (size_t i = 0; i < t->num_cols; ++i) {
    t->locs[i].offset = cur_offset;
    t->locs[i].length = sizeof(byte);
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

  InsertTuple(0, t1);
  InsertTuple(0, t2);

  ColDesc* table2_tuple_desc = NULL;
  ColDesc table2_col1 = {.column_name = "a", .type = T_STRING};
  arrpush(table2_tuple_desc, table2_col1);
  TableDef table_def2 = {.name = "b", .tuple_desc = table2_tuple_desc};
  CreateTable(&table_def2);

  Tuple* table2_t1 = MakeTuple(&table_def2);
  table2_t1 = SetCol(table2_t1, "a", MakeDatum(T_STRING, strdup("asdf")), &table_def2);
  InsertTuple(1, table2_t1);
  Tuple* table2_t2 = MakeTuple(&table_def2);
  table2_t2 = SetCol(table2_t2, "a", MakeDatum(T_STRING, strdup("cab")), &table_def2);
  InsertTuple(1, table2_t2);
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
  for (; i < arrlen(TableDefs); ++i) {
    if (strcmp(TableDefs[i].name, table_name) == 0) {
      table_def = &TableDefs[i];
      break;
    }
  }

  // TODO(ryan): Just return from break and return null with no check here.
  return i == arrlen(TableDefs) ? NULL : table_def;
}

BType GetColType(TableDef* table_def, const char* col_name) {
  for (size_t i = 0; i < arrlen(table_def->tuple_desc); ++i) {
    if (strcmp(col_name, table_def->tuple_desc[i].column_name) == 0) {
      return table_def->tuple_desc[i].type;
    }
  }
  return T_UNKNOWN;
}

// void WriteField(MemTuple* mtuple, byte* data, size_t size) {
//   assert(mtuple != NULL);
//   assert(mtuple->htuple != NULL);
//   assert(data != NULL);
//   assert(size < mtuple->size);
//   memcpy(mtuple->htuple, data, size);
//   return;
// }
size_t GetColIdx(Tuple* tuple, const char* col_name, TableDef* table_def, bool* is_missing) {
  assert(tuple != NULL);
  assert(table_def != NULL);
  assert(table_def->tuple_desc != NULL);
  assert(is_missing != NULL);
  for (size_t i = 0; i < arrlen(table_def->tuple_desc); ++i) {
    if (strcmp(table_def->tuple_desc[i].column_name, col_name) == 0) {
      *is_missing = false;
      return i;
    }
  }

  *is_missing = true;
  return 0;
}

Datum* GetCol(Tuple* tuple, const char* col_name, TableDef* table_def) {
  assert(tuple != NULL);
  assert(table_def != NULL);
  assert(table_def->tuple_desc != NULL);

  bool is_missing = false;
  size_t i = GetColIdx(tuple, col_name, table_def, &is_missing);
  if (is_missing) {
    return NULL;
  }

  DataLoc loc = tuple->locs[i];
  byte* data = tuple->data_offset + loc.offset;
  Datum* datum = calloc(1, sizeof(Datum));
  datum->type = table_def->tuple_desc[i].type;
  datum->length = loc.length;
  datum->data = data;
  return datum;
}

Tuple* SetCol(Tuple* tuple, const char* col_name, Datum datum, TableDef* table_def) {
  bool is_missing = false;
  size_t change_idx = GetColIdx(tuple, col_name, table_def, &is_missing);
  assert(!is_missing);

  size_t new_size = tuple->length + datum.length - tuple->locs[change_idx].length;
  Tuple* new_tuple = calloc(1, new_size);
  *new_tuple = *tuple;
  new_tuple->length = new_size;

  // TODO: Is this right? Don't have to alloc whole new thing if data is less than or equal to
  // datum length;
  byte* old_data_ptr = (byte*)tuple + tuple->data_offset;
  byte* new_data_ptr = (byte*)new_tuple + new_tuple->data_offset;
  size_t cur_offset = 0;
  for (size_t i = 0; i < new_tuple->num_cols; ++i) {
    if (i == change_idx) {
      size_t length = datum.length;
      new_tuple->locs[i].offset = cur_offset;
      new_tuple->locs[i].length = length;
      memcpy(new_data_ptr + cur_offset, datum.data, length);
    } else {
      size_t length = tuple->locs[i].length;
      new_tuple->locs[i].offset = cur_offset;
      new_tuple->locs[i].length = length;
      memcpy(new_data_ptr + cur_offset, old_data_ptr + tuple->locs[i].offset, length);
    }

    cur_offset += new_tuple->locs[i].length;
  }
  // free(tuple);
  return new_tuple;
}

Tuple* CopyTuple(Tuple* tuple, TableDef* table_def) {
  Tuple* new_tuple = MakeTuple(table_def);
  for (size_t i = 0; i < arrlen(table_def->tuple_desc); ++i) {
    const char* col_name = table_def->tuple_desc[i].column_name;
    Datum* col_data = GetCol(tuple, col_name, table_def);
    new_tuple = SetCol(new_tuple, col_name, *col_data, table_def);
  }
  return new_tuple;
}

void InsertTuple(size_t index, Tuple* tuple) {
  arrpush(Tables[index], tuple);
  return;
}

Tuple* GetTuple(size_t table_index, size_t index) {
  // NOTE: Need to do this check or else I could run off the end of the dynamic array.
  // arrdel() doesn't automatically clear the moved over spaces.
  if (index >= arrlen(Tables[table_index])) {
    return NULL;
  }
  return Tables[table_index][index];
}

void UpdateTuple(size_t table_index, Tuple* tuple, size_t index) {
  if (index >= arrlen(Tables[table_index])) {
    return;
  }

  Tuple* old_tuple = GetTuple(table_index, index);
  assert(old_tuple != NULL);
  *old_tuple = *tuple;
  return;
}

void DeleteHeapTuple(size_t table_index, size_t index) {
  assert(Tables[table_index] != NULL);
  arrdel(Tables[table_index], index);
}

void CreateTable(TableDef* table_def) {
  assert(table_def != NULL);
  table_def->index = arrlen(Tables);
  arrpush(TableDefs, *table_def);
  arrpush(Tables, NULL);
}
