#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void Panic(const char* msg);

bool* BoolDup(bool*);

#ifdef __cplusplus
}
#endif

#endif  // UTILS_HH
