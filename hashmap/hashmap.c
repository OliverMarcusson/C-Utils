#include "hashmap.h"
#include "../vec/vec.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct HashMapData HashMapData;
typedef struct _HMKVPair _HMKVPair;

// TEMP
typedef struct LinkedList LinkedList;
struct LinkedList {};

struct HashMapData {
  Vec items;
  size_t len;
  size_t cap;
  size_t value_size;
};

struct _HMKVPair {
  char *key;
  void *value;
  size_t value_size;
};

void _put(HashMap *m, char *key, void *value);
void *_get(HashMap *m, char *key);
void *_remove(HashMap *m, char *key);
void _remove_into(HashMap *m, void *dst, char *key);
size_t _len(HashMap *m);
size_t _cap(HashMap *m);
size_t _val_size(HashMap *m);

HashMap hashmap_new(size_t value_size, _VecDropFn drop, _VecPrintFn print) {
  HashMap hm;
  HashMapData *hmd = malloc(sizeof(HashMapData));

  hmd->items = vec_new(sizeof(LinkedList), drop, print);
  hmd->len = 0;
  hmd->cap = 16;
  hmd->value_size = value_size;

  hm._data = hmd;

  hm.put = _put;
  hm.get = _get;
  hm.remove = _remove;
  hm.remove_into = _remove_into;
  hm.len = _len;
  hm.capacity = _cap;
  hm.value_size = _val_size;

  return hm;
}
