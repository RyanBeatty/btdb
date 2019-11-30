#ifndef STORAGE_H
#define STORAGE_H
#include <unordered_map>

#include "collections.h"
#include "types.h"

typedef struct ColDesc {
  const char* column_name;
  BType type;
} ColDesc;

struct TableDef {
  const char* name;
  ColDesc* tuple_desc;  // stretchy buffer
};

TableDef* MakeTableDef(const char*, ColDesc*);

extern TableDef* Tables;  // sb

TableDef* FindTableDef(const char*);

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

VEC_PROTOTYPE(TuplePair, TuplePair);

typedef TuplePairVec Tuple;

Datum* GetCol(Tuple*, const char*);
void SetCol(Tuple*, const char*, Datum);
Tuple* CopyTuple(Tuple*);

// TODO: Figure out what a tuple will actually look like.
// typedef std::unordered_map<std::string, Datum> Tuple;

VEC_PROTOTYPE(TuplePtr, Tuple*);

extern TuplePtrVec* Tuples;

void InsertTuple(Tuple*);
void UpdateTuple(Tuple*, size_t);
TuplePtrVecIt GetTuple(size_t);
void DeleteHeapTuple(size_t);

#endif  // STORAGE_H
