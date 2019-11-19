#ifndef STORAGE_H
#define STORAGE_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "collections.h"

namespace btdb {

struct TableDef {
  std::string name;
  std::vector<std::string> col_names;
};

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
typedef std::unordered_map<std::string, std::string> Tuple;
typedef std::unique_ptr<Tuple> MTuple;

VEC_PROTOTYPE(TuplePtr, Tuple*);

static TuplePtrVec* Tuples = MakeTuplePtrVec();

void InsertTuple(Tuple*);
void UpdateTuple(Tuple*, size_t);
TuplePtrVecIt GetTuple(size_t);
void DeleteHeapTuple(size_t);


// static std::vector<Tuple> Tuples;

}  // namespace btdb
#endif  // STORAGE_H
