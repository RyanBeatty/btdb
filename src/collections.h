#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

namespace btdb {

#define VEC_FOREACH(it, vec) \
  for (it = &vec->buffer[0]; it != (vec->buffer + vec->length); ++it)

#define VEC_PROTOTYPE(name, type)                                   \
  struct name##Vec {                                                \
    size_t length;                                                  \
    size_t capacity;                                                \
    type* buffer;                                                   \
  };                                                                \
                                                                    \
  typedef type* name##VecIt;                                        \
                                                                    \
  name##Vec* Make##name##Vec() {                                    \
    name##Vec* vec = (name##Vec*)calloc(1, sizeof(name##Vec));      \
    assert(vec != NULL);                                            \
    vec->buffer = (type*)calloc(10, sizeof(type));                  \
    vec->capacity = 10;                                             \
    return vec;                                                     \
  }                                                                 \
                                                                    \
  void PushBack(name##Vec* vec, type data) {                        \
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
  type* Get(name##Vec* vec, size_t i) {                             \
    assert(vec != NULL);                                            \
    assert(vec->buffer != NULL);                                    \
    if (i >= vec->length) {                                         \
      return NULL;                                                  \
    }                                                               \
    return &vec->buffer[i];                                         \
  }                                                                 \
                                                                    \
  void free_vec(name##Vec* vec) {                                   \
    if (vec == NULL) {                                              \
      return;                                                       \
    }                                                               \
    free(vec->buffer);                                              \
    free(vec);                                                      \
    vec = NULL;                                                     \
    return;                                                         \
  }

VEC_PROTOTYPE(CharPtr, char*);

}  // namespace btdb

#endif  // COLLECTIONS_H