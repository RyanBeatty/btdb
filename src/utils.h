#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void Panic(const char* msg);

bool* BoolDup(bool*);


#define BoolToStr(b) b ? "true" : "false"

#ifdef __cplusplus
}
#endif

#endif  // UTILS_HH
