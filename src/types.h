#ifndef TYPES_H
#define TYPES_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace btdb {

enum BType {
  T_UNKNOWN,
  T_STRING,
  T_BOOL,
};

struct Datum {
  // Datum(BType type, void* data) : type(type), data(data) {}

  BType type;
  // TODO(ryan): REMEMBER TO FIGURE OUT BEST WAY TO DELETE THIS, WE LEAK MEM HERE.
  void* data;
};

Datum MakeDatum(BType, void*);

}  // namespace btdb
#endif  // TYPES_H
