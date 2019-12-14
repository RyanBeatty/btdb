#ifndef STORAGE_H
#define STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

typedef struct ColDesc {
  const char* column_name;
  BType type;
} ColDesc;

typedef struct TableDef {
  const char* name;
  ColDesc* tuple_desc;  // sbarr
} TableDef;

TableDef* MakeTableDef(const char*, ColDesc*);

extern TableDef* TableDefs;  // sbarr

TableDef* FindTableDef(const char*);
BType GetColType(TableDef*, const char*);

// typedef unsigned char byte;
// typedef byte* HeapTuple;

// VEC_PROTOTYPE(HeapTuple, HeapTuple);
// static HeapTupleVec* Buffer = MakeHeapTupleVec();

// struct MemTuple {
//   size_t size;
//   HeapTuple htuple;
// };

// void WriteField(MemTuple* mtuple, byte* data, size_t size);

typedef struct TuplePair {
  char* column_name;
  Datum data;
} TuplePair;

typedef TuplePair Tuple;  // sbarr

Datum* GetCol(Tuple*, const char*);
Tuple* SetCol(Tuple*, const char*, Datum);
Tuple* CopyTuple(Tuple*);

// TODO: Figure out what a tuple will actually look like.
// typedef std::unordered_map<std::string, Datum> Tuple;

// VEC_PROTOTYPE(TuplePtr, Tuple*);

extern Tuple** Tuples;  // sbarr

void InsertTuple(Tuple*);
void UpdateTuple(Tuple*, size_t);
Tuple* GetTuple(size_t);
void DeleteHeapTuple(size_t);

typedef Tuple** Table;

// extern Table* TableDefs;  // sbarr

#ifdef __cplusplus
}
#endif

#endif  // STORAGE_H
