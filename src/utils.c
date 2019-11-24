#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void Panic(const char* msg) {
  fprintf(stderr, "Panic: %s\n", msg);
  exit(EXIT_FAILURE);
}
