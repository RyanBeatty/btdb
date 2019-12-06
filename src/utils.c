#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void Panic(const char* msg) {
  fprintf(stderr, "Panic: %s\n", msg);
  exit(EXIT_FAILURE);
}

bool* BoolDup(bool* b) {
  if (b == NULL) {
    return NULL;
  }

  bool* new_b = calloc(1, sizeof(bool));
  *new_b = *b;
  return new_b;
}
