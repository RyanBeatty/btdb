#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

namespace btdb {

#define VEC_FOREACH(it, vec) \
  for (it = &vec->buffer[0]; it != (vec->buffer + vec->length); ++it)

#define VEC_PROTOTYPE(name, type)                                   \
  struct name##_vec {                                               \
    size_t length;                                                  \
    size_t capacity;                                                \
    type* buffer;                                                   \
  };                                                                \
                                                                    \
  name##_vec* make_##name##_vec() {                                 \
    name##_vec* vec = (name##_vec*)calloc(10, sizeof(type));        \
    assert(vec != NULL);                                            \
    vec->capacity = 10;                                             \
    return vec;                                                     \
  }                                                                 \
                                                                    \
  void push(name##_vec* vec, type data) {                           \
    assert(vec != NULL);                                            \
    assert(vec->buffer != NULL);                                    \
    if (vec->length >= vec->capacity) {                             \
      size_t new_capacity = 2 * vec->capacity * sizeof(type);       \
      type* new_buffer = (type*)realloc(vec->buffer, new_capacity); \
      assert(new_buffer != NULL);                                   \
      vec->buffer = new_buffer;                                     \
      vec->capacity = new_capacity;                                 \
    }                                                               \
    vec->buffer[vec->length] = data;                                \
    ++vec->length;                                                  \
    return;                                                         \
  }                                                                 \
                                                                    \
  type* get(name##_vec* vec, size_t i) {                            \
    assert(vec != NULL);                                            \
    assert(vec->buffer != NULL);                                    \
    if (i < 0 || i >= vec->length) {                                \
      return NULL;                                                  \
    }                                                               \
    return &vec->buffer[i];                                         \
  }                                                                 \
                                                                    \
  void free_vec(name##_vec* vec) {                                  \
    if (vec == NULL) {                                              \
      return;                                                       \
    }                                                               \
    free(vec->buffer);                                              \
    free(vec);                                                      \
    vec = NULL;                                                     \
    return;                                                         \
  }

VEC_PROTOTYPE(int, int);

}  // namespace btdb

#endif  // COLLECTIONS_H