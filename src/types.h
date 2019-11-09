#ifndef TYPES_H
#define TYPES_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace btdb {

struct TableDef {
  std::string name;
  std::vector<std::string> col_names;
};

// TODO: Figure out what a tuple will actually look like.
typedef std::unordered_map<std::string, std::string> Tuple;
typedef std::unique_ptr<Tuple> MTuple;

static std::vector<Tuple> Tuples;

enum BType {
  T_UNKNOWN,
  T_STRING,
  T_BOOL,
};

struct Datum {
  Datum(BType type, void* data) : type(type), data(data) {}

  BType type;
  // TODO(ryan): REMEMBER TO FIGURE OUT BEST WAY TO DELETE THIS, WE LEAK MEM HERE.
  void* data;
};

}  // namespace btdb
#endif  // TYPES_H
