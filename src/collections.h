#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define VEC_FOREACH(it, vec) \
  for (it = &vec->buffer[0]; it != (vec->buffer + vec->length); ++it)

#define VEC_LENGTH(vec_ptr) vec_ptr->length
#define VEC_BUFFER(vec_ptr) vec_ptr->buffer

#define VEC_PROTOTYPE(name, type)                                   \
  typedef struct name##Vec {                                        \
    size_t length;                                                  \
    size_t capacity;                                                \
    type* buffer;                                                   \
  } name##Vec;                                                      \
                                                                    \
  typedef type* name##VecIt;                                        \
                                                                    \
  inline name##Vec* Make##name##Vec() {                             \
    name##Vec* vec = (name##Vec*)calloc(1, sizeof(name##Vec));      \
    assert(vec != NULL);                                            \
    vec->buffer = (type*)calloc(10, sizeof(type));                  \
    vec->capacity = 10;                                             \
    return vec;                                                     \
  }                                                                 \
                                                                    \
  inline void PushBack(name##Vec* vec, type data) {                 \
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
  inline void Erase(name##Vec* vec, size_t index) {                 \
    assert(vec != NULL);                                            \
    assert(vec->buffer != NULL);                                    \
    assert(index < vec->length);                                    \
    vec->buffer[index] = vec->buffer[vec->length - 1];              \
    memset(&vec->buffer[vec->length - 1], (int)0, sizeof(type));    \
    --vec->length;                                                  \
  }                                                                 \
                                                                    \
  inline type* Get(name##Vec* vec, size_t i) {                      \
    assert(vec != NULL);                                            \
    assert(vec->buffer != NULL);                                    \
    if (i >= vec->length) {                                         \
      return NULL;                                                  \
    }                                                               \
    return &vec->buffer[i];                                         \
  }                                                                 \
                                                                    \
  inline void free_vec(name##Vec* vec) {                            \
    if (vec == NULL) {                                              \
      return;                                                       \
    }                                                               \
    free(vec->buffer);                                              \
    free(vec);                                                      \
    vec = NULL;                                                     \
    return;                                                         \
  }

#define MAP_PROTOTYPE(name, key_t, val_t, hash_func, cmp_func)         \
  typedef struct name##MapEntry {                                      \
    key_t key;                                                         \
    val_t val;                                                         \
    bool is_filled;                                                    \
  } name##MapEntry;                                                    \
                                                                       \
  typedef struct name##Map {                                           \
    size_t size;                                                       \
    size_t capacity;                                                   \
    struct name##MapEntry* buffer;                                     \
  } name##Map;                                                         \
                                                                       \
  inline name##Map* Make##name##Map() {                                \
    name##Map* map = (name##Map*)calloc(1, sizeof(name##Map));         \
    map->buffer = (name##MapEntry*)calloc(10, sizeof(name##MapEntry)); \
    map->size = 0;                                                     \
    map->capacity = 10;                                                \
  }                                                                    \
                                                                       \
  inline name##MapEntry* MapGet(name##Map* map, key_t key) {           \
    if (!map->size) {                                                  \
      return NULL;                                                     \
    }                                                                  \
    size_t hash = hash_func(key);                                      \
    size_t i = hash % map->capacity;                                   \
    size_t end = (i - 1 + map->capacity) % map->capacity;              \
    for (; i != end; ++i % map->capacity) {                            \
      name##MapEntry* entry = &map->buffer[i];                         \
      if (entry->is_filled && cmp_func(key, entry->key)) {             \
        return entry;                                                  \
      }                                                                \
    }                                                                  \
    return NULL;                                                       \
  }                                                                    \
                                                                       \
  inline name##MapEntry* MapSet(name##Map* map, key_t key) {           \
    assert(map->size < map->capacity);                                 \
    size_t hash = hash_func(key);                                      \
    size_t i = hash % map->capacity;                                   \
    size_t end = (i - 1 + map->capacity) % map->capacity;              \
    for (; i != end; ++i % map->capacity) {                            \
      name##MapEntry* entry = &map->buffer[i];                         \
      if (!entry->is_filled) {                                         \
        return entry;                                                  \
      }                                                                \
      if (cmp_func(key, entry->key)) {                                 \
        return entry;                                                  \
      }                                                                \
    }                                                                  \
    return NULL;                                                       \
  }

// http://www.cse.yorku.ca/~oz/hash.html
// size_t hash_djb2(unsigned char* str) {
//   size_t hash = 5381;
//   int c;

//   while (c = *str++) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

//   return hash;
// }

// MAP_PROTOTYPE(StrStr, char*, char*, hash_djb2, strcmp);

VEC_PROTOTYPE(CharPtr, char*);

#ifdef __cplusplus
}
#endif

#endif  // COLLECTIONS_H