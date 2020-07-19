#ifndef DRIVER_H
#define DRIVER_H
#include <string.h>

#include "node.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct yy_buffer_state* YY_BUFFER_STATE;

typedef struct Parser {
  YY_BUFFER_STATE buffer_state;
  ParseNode* tree;
} Parser;

// Defined in scanner.
Parser* InitParser(char*);
// Defined in scanner.
void FreeParser(Parser*);
int Parse(Parser*);

#ifdef __cplusplus
}
#endif

#endif  // DRIVER_H
