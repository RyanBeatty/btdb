#include "storage.h"
#include "collections.h"

namespace btdb {

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
        return ;
    }

    TuplePtrVecIt it = GetTuple(index);
    assert(it != NULL);
    *it = tuple;
    return ;    
}

void DeleteHeapTuple(size_t index) { Erase(Tuples, index); }

}  // namespace btdb