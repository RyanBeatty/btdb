#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "utils.h"

BType StringToType(const char* type_str) {
  if (type_str == NULL) {
    return T_UNKNOWN;
  }
  if (strcmp(type_str, "text") == 0) {
    return T_STRING;
  } else if (strcmp(type_str, "bool") == 0) {
    return T_BOOL;
  } else if (strcmp(type_str, "int") == 0) {
    return T_INT;
  } else {
    return T_UNKNOWN;
  }
}

Datum MakeDatum(BType type, void* data) {
  Datum datum = {type, data};
  return datum;
}

Datum BoolEQ(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*)d1.data) == *((bool*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolNE(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*)d1.data) != *((bool*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolGT(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*)d1.data) > *((bool*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolLT(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*)d1.data) < *((bool*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolGTE(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*)d1.data) >= *((bool*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolLTE(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*)d1.data) <= *((bool*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolAnd(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*)d1.data) && *((bool*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolOr(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*)d1.data) || *((bool*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolNot(Datum d1) {
  assert(d1.type == T_BOOL);
  assert(d1.data != NULL);
  bool result = !*((bool*)d1.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum StrEQ(Datum d1, Datum d2) {
  assert(d1.type == T_STRING);
  assert(d2.type == T_STRING);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = strcmp((char*)d1.data, ((char*)d2.data)) == 0;
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum StrNE(Datum d1, Datum d2) {
  assert(d1.type == T_STRING);
  assert(d2.type == T_STRING);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = strcmp((char*)d1.data, ((char*)d2.data)) != 0;
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum StrGT(Datum d1, Datum d2) {
  assert(d1.type == T_STRING);
  assert(d2.type == T_STRING);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = strcmp((char*)d1.data, ((char*)d2.data)) > 0;
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum StrLT(Datum d1, Datum d2) {
  assert(d1.type == T_STRING);
  assert(d2.type == T_STRING);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = strcmp((char*)d1.data, ((char*)d2.data)) < 0;
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum StrGTE(Datum d1, Datum d2) {
  assert(d1.type == T_STRING);
  assert(d2.type == T_STRING);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = strcmp((char*)d1.data, ((char*)d2.data)) >= 0;
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum StrLTE(Datum d1, Datum d2) {
  assert(d1.type == T_STRING);
  assert(d2.type == T_STRING);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = strcmp((char*)d1.data, ((char*)d2.data)) <= 0;
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum IntEQ(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((int32_t*)d1.data) == *((int32_t*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}
Datum IntNE(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((int32_t*)d1.data) != *((int32_t*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}
Datum IntGT(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((int32_t*)d1.data) > *((int32_t*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}
Datum IntLT(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((int32_t*)d1.data) < *((int32_t*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}
Datum IntGTE(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((int32_t*)d1.data) >= *((int32_t*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}
Datum IntLTE(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((int32_t*)d1.data) <= *((int32_t*)d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}
int IntCmp(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  return *((int32_t*)d2.data) - *((int32_t*)d1.data);
}

Datum IntAdd(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  int32_t* result = (int32_t*)calloc(1, sizeof(int32_t));
  *result = *((int32_t*)d1.data) + *((int32_t*)d2.data);
  return MakeDatum(T_INT, result);
}
Datum IntSub(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  int32_t* result = (int32_t*)calloc(1, sizeof(int32_t));
  *result = *((int32_t*)d1.data) - *((int32_t*)d2.data);
  return MakeDatum(T_INT, result);
}
Datum IntMult(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  int32_t* result = (int32_t*)calloc(1, sizeof(int32_t));
  *result = *((int32_t*)d1.data) * *((int32_t*)d2.data);
  return MakeDatum(T_INT, result);
}
Datum IntDiv(Datum d1, Datum d2) {
  assert(d1.type == T_INT);
  assert(d2.type == T_INT);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  int32_t* result = (int32_t*)calloc(1, sizeof(int32_t));
  *result = *((int32_t*)d1.data) / *((int32_t*)d2.data);
  return MakeDatum(T_INT, result);
}
