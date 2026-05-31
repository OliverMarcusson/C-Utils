#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

typedef struct HashMap HashMap;

// A hashed key-value pair map.
// - Data is stored on the heap.
// - The map's `free` function must be called to free it.
struct HashMap {
  // The hashmap's internal data.
  // This is not part of the API and is not meant to be used.
  void *_data;

  // Puts a kv-pair in the hashmap.
  // Moves ownership of the data to the hashmap.
  void (*put)(HashMap *m, char *key, void *value);

  // Returns a pointer to the value associated with the provided key.
  // Is only safe to use until the map is mutated.
  // Do not free this.
  void *(*get)(HashMap *m, char *key);

  // Removes the kv-pair from the hashmap and returns the associated value.
  // Returned value is heap allocated.
  // Moves ownership of the value to the caller.
  void *(*remove)(HashMap *m, char *key);

  // Removes the kv-pair from the hashmap,
  // and copies the value into the provided buffer.
  // The caller must allocate enough space for the value in `dst`.
  // Moves ownership of the value to the caller.
  void (*remove_into)(HashMap *m, void *dst, char *key);

  // Returns the amount of items in the map.
  size_t (*len)(HashMap *m);

  // Returns the current max capacity of the map.
  size_t (*capacity)(HashMap *m);

  // Returns the size of the values in the map.
  size_t (*value_size)(HashMap *m);
};

#endif // !HASHMAP_H
