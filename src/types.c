#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "types.h"
#include "utils.h"

Datum MakeDatum(BType type, void* data) {
  Datum datum = {type, data};
  return datum;
}

Datum BoolEQ(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*) d1.data) == *((bool*) d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolNE(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*) d1.data) != *((bool*) d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolGT(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*) d1.data) < *((bool*) d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolLT(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*) d1.data) < *((bool*) d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));  
}

Datum BoolGTE(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*) d1.data) >= *((bool*) d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));  
}

Datum BoolLTE(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*) d1.data) <= *((bool*) d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));  
}

Datum BoolAnd(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*) d1.data) && *((bool*) d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolOr(Datum d1, Datum d2) {
  assert(d1.type == T_BOOL);
  assert(d2.type == T_BOOL);
  assert(d1.data != NULL);
  assert(d2.data != NULL);
  bool result = *((bool*) d1.data) || *((bool*) d2.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}

Datum BoolNot(Datum d1) {
  assert(d1.type == T_BOOL);
  assert(d1.data != NULL);
  bool result = !*((bool*) d1.data);
  return MakeDatum(T_BOOL, BoolDup(&result));
}
