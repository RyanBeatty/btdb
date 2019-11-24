#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BType {
  T_UNKNOWN,
  T_STRING,
  T_BOOL,
} BType;

typedef struct Datum {
  // Datum(BType type, void* data) : type(type), data(data) {}

  enum BType type;
  // TODO(ryan): REMEMBER TO FIGURE OUT BEST WAY TO DELETE THIS, WE LEAK MEM HERE.
  void* data;
} Datum;

Datum MakeDatum(BType, void*);

#ifdef __cplusplus
}
#endif

#endif  // TYPES_H
