#include "stb_ds.h"

#include "storage.h"

TableDef* Tables = NULL;
Tuple** Tuples = NULL;

TableDef* MakeTableDef(const char* name, ColDesc* tuple_desc) {
  size_t len = strlen(name);
  char* cpy = (char*)calloc(sizeof(char), len);
  strncpy(cpy, name, len);
  TableDef* def = (TableDef*)calloc(sizeof(TableDef), 1);
  def->name = cpy;
  def->tuple_desc = tuple_desc;
  return def;
}

TableDef* FindTableDef(const char* table_name) {
  TableDef* table_def = NULL;
  size_t i = 0;
  for (; i < arrlen(Tables); ++i) {
    if (strcmp(Tables[i].name, table_name) == 0) {
      table_def = &Tables[i];
      break;
    }
  }

  // TODO(ryan): Just return from break and return null with no check here.
  return i == arrlen(Tables) ? NULL : table_def;
}

// void WriteField(MemTuple* mtuple, byte* data, size_t size) {
//   assert(mtuple != NULL);
//   assert(mtuple->htuple != NULL);
//   assert(data != NULL);
//   assert(size < mtuple->size);
//   memcpy(mtuple->htuple, data, size);
//   return;
// }

Datum* GetCol(Tuple* tuple, const char* col_name) {
  TuplePair* pair = NULL;
  size_t i = 0;
  for (; i < VEC_END(tuple); ++i) {
    pair = &VEC_VALUE(tuple, i);
    assert(pair != NULL);
    if (strcmp(pair->column_name, col_name) == 0) {
      return &pair->data;
    }
  }

  return NULL;
}

void SetCol(Tuple* tuple, const char* col_name, Datum data) {
  Datum* old_data = GetCol(tuple, col_name);
  if (old_data == NULL) {
    TuplePair pair;
    pair.column_name = (char*)calloc(sizeof(char), strlen(col_name));
    strncpy(pair.column_name, col_name, strlen(col_name));
    pair.data = data;
    PushBack(tuple, pair);
    return;
  }
  *old_data = data;
}

Tuple* CopyTuple(Tuple* tuple) {
  Tuple* new_tuple = MakeTuplePairVec();
  TuplePair pair;
  for (size_t i = 0; i < VEC_END(tuple); ++i) {
    pair = VEC_VALUE(tuple, i);
    SetCol(new_tuple, pair.column_name, pair.data);
  }
  return new_tuple;
}

void InsertTuple(Tuple* tuple) {
  arrpush(Tuples, tuple);
  return;
}

Tuple* GetTuple(size_t index) {
  // NOTE: Need to do this check or else I could run off the end of the dynamic array.
  // arrdel() doesn't automatically clear the moved over spaces.
  if (index >= arrlen(Tuples)) {
    return NULL;
  }
  return Tuples[index];
}

void UpdateTuple(Tuple* tuple, size_t index) {
  if (index >= arrlen(Tuples)) {
    return;
  }

  Tuple* old_tuple = GetTuple(index);
  assert(old_tuple != NULL);
  *old_tuple = *tuple;
  return;
}

void DeleteHeapTuple(size_t index) { arrdel(Tuples, index); }
