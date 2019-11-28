#include "storage.h"
#include "collections.h"

TableDefPtrVec* Tables = MakeTableDefPtrVec();

TableDef* MakeTableDef(const char* name, std::unordered_map<std::string, BType> tuple_desc) {
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
  for (; i < VEC_END(Tables); ++i) {
    table_def = VEC_VALUE(Tables, i);
    assert(table_def != NULL);
    if (strcmp(table_def->name, table_name) == 0) {
      break;
    }
  }

  return i == VEC_END(Tables) ? NULL : table_def;
}

// void WriteField(MemTuple* mtuple, byte* data, size_t size) {
//   assert(mtuple != NULL);
//   assert(mtuple->htuple != NULL);
//   assert(data != NULL);
//   assert(size < mtuple->size);
//   memcpy(mtuple->htuple, data, size);
//   return;
// }

void InsertTuple(Tuple* tuple) {
  PushBack(Tuples, tuple);
  return;
}

TuplePtrVecIt GetTuple(size_t index) { return Get(Tuples, index); }

void UpdateTuple(Tuple* tuple, size_t index) {
  if (index >= VEC_LENGTH(Tuples)) {
    return;
  }

  TuplePtrVecIt it = GetTuple(index);
  assert(it != NULL);
  *it = tuple;
  return;
}

void DeleteHeapTuple(size_t index) { Erase(Tuples, index); }
