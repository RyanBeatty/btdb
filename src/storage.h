#ifndef STORAGE_H
#define STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "types.h"

typedef struct ColDesc {
  const char* column_name;
  BType type;
} ColDesc;

typedef struct TableDef {
  const char* name;
  ColDesc* tuple_desc;  // sbarr
  size_t index;
} TableDef;

TableDef* MakeTableDef(const char*, ColDesc*, size_t);

extern TableDef* TableDefs;  // sbarr

TableDef* FindTableDef(const char*);
BType GetColType(TableDef*, const char*);

typedef struct TuplePair {
  char* column_name;
  Datum data;
} TuplePair;

typedef struct Tuple {
  size_t num_cols;
  TuplePair data[];
} Tuple;

Tuple* MakeTuple(TableDef*);
Datum* GetCol(Tuple*, const char*, TableDef*);
Tuple* SetCol(Tuple*, const char*, Datum, TableDef*);
Tuple* CopyTuple(Tuple*, TableDef*);

typedef Tuple** Table;

extern Table* Tables;  // sbarr

void InsertTuple(size_t, Tuple*);
// TODO: This appears to be unused.
void UpdateTuple(size_t, Tuple*, size_t);
Tuple* GetTuple(size_t, size_t);
void DeleteHeapTuple(size_t, size_t);

void CreateTable(TableDef*);

void InitSystemTables();

#ifdef __cplusplus
}
#endif

#endif  // STORAGE_H
