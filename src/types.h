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

BType StringToType(const char*);

typedef struct Datum {
  // Datum(BType type, void* data) : type(type), data(data) {}

  enum BType type;
  // TODO(ryan): REMEMBER TO FIGURE OUT BEST WAY TO DELETE THIS, WE LEAK MEM HERE.
  void* data;
} Datum;

Datum MakeDatum(BType, void*);

Datum BoolEQ(Datum, Datum);
Datum BoolNE(Datum, Datum);
Datum BoolGT(Datum, Datum);
Datum BoolLT(Datum, Datum);
Datum BoolGTE(Datum, Datum);
Datum BoolLTE(Datum, Datum);
Datum BoolAnd(Datum, Datum);
Datum BoolOr(Datum, Datum);
Datum BoolNot(Datum);
int BoolCmp(Datum, Datum);

Datum StrEQ(Datum, Datum);
Datum StrNE(Datum, Datum);
Datum StrGT(Datum, Datum);
Datum StrLT(Datum, Datum);
Datum StrGTE(Datum, Datum);
Datum StrLTE(Datum, Datum);
Datum StrAnd(Datum, Datum);
Datum StrOr(Datum, Datum);
Datum StrNot(Datum);
int StrCmp(Datum, Datum);

#ifdef __cplusplus
}
#endif

#endif  // TYPES_H
