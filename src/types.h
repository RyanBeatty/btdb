#ifndef TYPES_H
#define TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef enum BType {
  T_UNKNOWN,
  T_STRING,
  T_BOOL,
  T_INT,
  T_NULL,
} BType;

BType StringToType(const char*);
const char* TypeToString(BType);

typedef struct Datum {
  enum BType type;
  size_t length;
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
int StrCmp(Datum, Datum);
// TODO(ryan): Stop hardcoding ops in parser.
Datum StrCat(Datum, Datum);

Datum IntEQ(Datum, Datum);
Datum IntNE(Datum, Datum);
Datum IntGT(Datum, Datum);
Datum IntLT(Datum, Datum);
Datum IntGTE(Datum, Datum);
Datum IntLTE(Datum, Datum);
int IntCmp(Datum, Datum);
Datum IntAdd(Datum, Datum);
Datum IntSub(Datum, Datum);
Datum IntMult(Datum, Datum);
Datum IntDiv(Datum, Datum);

#define GetBoolResult(datum) (*((bool*)datum.data))

#ifdef __cplusplus
}
#endif

#endif  // TYPES_H
