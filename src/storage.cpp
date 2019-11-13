#include "storage.h"
#include "collections.h"

namespace btdb {

void WriteHeapTuple(HeapTuple tuple, size_t index) {
  size_t length = VEC_LENGTH(Buffer);
  if (index < length) {
    HeapTupleVecIt item = Get(Buffer, index);
    *item = tuple;
    return;
  }

  PushBack(Buffer, tuple);
  return;
}

HeapTupleVecIt GetHeapTuple(size_t index) { return Get(Buffer, index); }

void DeleteHeapTuple(size_t index) { Erase(Buffer, index); }

}  // namespace btdb