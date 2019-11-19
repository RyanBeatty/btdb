#include "types.h"

namespace btdb {
Datum MakeDatum(BType type, void* data) {
    return Datum{
        type,
        data
    };
}
}