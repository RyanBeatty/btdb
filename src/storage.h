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

typedef unsigned char* HeapTuple;

VEC_PROTOTYPE(HeapTuple, HeapTuple);

static HeapTupleVec Tuples2;

// TODO: Figure out what a tuple will actually look like.
typedef std::unordered_map<std::string, std::string> Tuple;
typedef std::unique_ptr<Tuple> MTuple;

static std::vector<Tuple> Tuples;

}  // namespace btdb
#endif  // STORAGE_H
