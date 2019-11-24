#include "types.h"

Datum MakeDatum(BType type, void* data) { return Datum{type, data}; }