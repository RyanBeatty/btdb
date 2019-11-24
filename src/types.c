#include "types.h"

Datum MakeDatum(BType type, void* data) {
  Datum datum = {type, data};
  return datum;
}