#ifndef STORAGE_H
#define STORAGE_H
#include <memory>
#include <unordered_map>

#include "collections.h"
#include "types.h"

struct TableDef {
  const char* name;
  std::unordered_map<std::string, BType> tuple_desc;
};

TableDef* MakeTableDef(const char*, std::unordered_map<std::string, BType>);

VEC_PROTOTYPE(TableDefPtr, TableDef*);
extern TableDefPtrVec* Tables;

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

// TODO: Figure out what a tuple will actually look like.
typedef std::unordered_map<std::string, Datum> Tuple;
typedef std::unique_ptr<Tuple> MTuple;

VEC_PROTOTYPE(TuplePtr, Tuple*);

static TuplePtrVec* Tuples = MakeTuplePtrVec();

void InsertTuple(Tuple*);
void UpdateTuple(Tuple*, size_t);
TuplePtrVecIt GetTuple(size_t);
void DeleteHeapTuple(size_t);

#endif  // STORAGE_H
