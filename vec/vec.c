#include "vec.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  void *items;
  size_t len;
  size_t cap;
  size_t item_size;
  _VecDropFn drop;
  _VecPrintFn print;
} VecData;

static VecData *_vec_data(Vec *v) { return v->_data; }

// Returns true if idx is a valid vector index and false if it's not.
// -1 is interpreted as the last item in a vector,
// and is valid.
static bool _is_valid_idx(Vec *v, int idx) {
  VecData *data = _vec_data(v);

  if (idx < -1) {
    return false;
  }

  if (idx == -1) {
    return data->len > 0;
  }

  if ((size_t)idx >= data->len) {
    return false;
  }

  return true;
}

static int _translate_idx(Vec *v, int idx) {
  if (idx == -1) {
    return _vec_data(v)->len - 1;
  }
  return idx;
}

// Returns a pointer to shallow copied data, transfering its ownership.
static void *_move_data(void *src, size_t len) {
  void *new_item = malloc(len);
  if (new_item == NULL) {
    perror("Memory allocation failed.");
    return NULL;
  }

  memcpy(new_item, src, len);
  return new_item;
}

// Doubles the capacity of the vector if it needs to increase.
// Returns true if the capacity has doubled or there was no need.
// Returns false if memory reallocation failed.
static bool _inc_vec_cap(Vec *v) {
  VecData *data = _vec_data(v);

  if (data->len == data->cap) {
    void *new_items = realloc(data->items, data->cap * data->item_size * 2);

    if (new_items == NULL) {
      perror("Failed to realloc memory for vec expansion.");
      return false;
    }

    data->cap *= 2;
    data->items = new_items;
    return true;
  }
  return true;
}

static void *_vec_get_ptr(Vec *v, int idx);
static size_t _vec_len(Vec *v);
static size_t _vec_cap(Vec *v);
static size_t _vec_item_size(Vec *v);
static void _vec_free(Vec *v);
static bool _vec_push(Vec *v, void *item);
static void *_vec_pop(Vec *v, int idx);
static void _vec_pop_into(Vec *v, void *dst, int idx);
static bool _vec_insert(Vec *v, void *item, int idx);
static void _vec_print(Vec *v);

// Returns a borrowed pointer to an item in the vector.
// Do not free this.
static void *_vec_get_ptr(Vec *v, int idx) {
  if (!_is_valid_idx(v, idx)) {
    return NULL;
  }

  idx = _translate_idx(v, idx);
  VecData *data = _vec_data(v);

  return (char *)data->items + idx * data->item_size;
}

static size_t _vec_len(Vec *v) { return _vec_data(v)->len; }

static size_t _vec_cap(Vec *v) { return _vec_data(v)->cap; }

static size_t _vec_item_size(Vec *v) { return _vec_data(v)->item_size; }

// Returns a new vector for a specific item size.
// Define `drop_fun` if items stored in the vec needs to be deconstructed on
// free. Must be freed with the vector's `free` method.
Vec vec_new(size_t item_size, _VecDropFn drop, _VecPrintFn print) {
  Vec v;
  VecData *data = malloc(sizeof(VecData));

  if (data == NULL) {
    perror("Failed to malloc memory for vec data.");
  }

  data->items = malloc(item_size * 8);

  if (data->items == NULL) {
    perror("Failed to malloc memory for vec items.");
  }

  data->len = 0;
  data->cap = 8;
  data->item_size = item_size;
  data->drop = drop;
  data->print = print;

  v._data = data;
  v.get_ptr = _vec_get_ptr;
  v.len = _vec_len;
  v.cap = _vec_cap;
  v.item_size = _vec_item_size;
  v.free = _vec_free;
  v.push = _vec_push;
  v.pop = _vec_pop;
  v.pop_into = _vec_pop_into;
  v.insert = _vec_insert;
  v.print = _vec_print;

  return v;
}

// Frees the vector and deconstructs its data if the vector's drop function is
// defined.
static void _vec_free(Vec *v) {
  VecData *data = _vec_data(v);

  if (data->drop != NULL) {
    for (size_t i = 0; i < data->len; i++) {
      if (data->items != NULL) {
        data->drop(_vec_get_ptr(v, (int)i));
      }
    }
  }
  free(data->items);
  free(data);
  v->_data = NULL;
}

// Pushes the item to the vector, moving its ownership to it.
// Item must be primitive or heap allocated.
static bool _vec_push(Vec *v, void *item) {
  if (!_inc_vec_cap(v)) {
    return false;
  }

  VecData *data = _vec_data(v);
  memcpy((char *)data->items + data->len * data->item_size, item,
         data->item_size);
  data->len++;
  return true;
}

// Returns a pointer to heap allocated data moved out of the vector.
// Moves the data's owenership to the caller.
static void *_vec_pop(Vec *v, int idx) {
  VecData *data = _vec_data(v);
  idx = _translate_idx(v, idx);
  bool mem_move = (size_t)idx != data->len - 1;

  char *item = _vec_get_ptr(v, idx);
  void *popped_item = _move_data(item, data->item_size);

  // Zero out the memory where the data used to reside.
  for (size_t i = 0; i < data->item_size; i++) {
    *(item + i) = '\0';
  }

  if (mem_move) {
    size_t size = (data->len - idx - 1) * data->item_size;
    memmove(item, item + data->item_size, size);
  }

  data->len--;

  return popped_item;
}

// Pops the item at index idx into the provided buffer dst, moving it's
// ownership. If data is heap allocated, it needs to be freed by the caller.
static void _vec_pop_into(Vec *v, void *dst, int idx) {
  void *popped_item = _vec_pop(v, idx);
  memcpy(dst, popped_item, _vec_data(v)->item_size);
  free(popped_item);
}

// Inserts an item at the specified index in the vector.
// Moves the ownership of the item to the vector.
static bool _vec_insert(Vec *v, void *item, int idx) {
  if (!_inc_vec_cap(v)) {
    return false;
  }

  if (!_is_valid_idx(v, idx)) {
    return false;
  }

  idx = _translate_idx(v, idx);
  void *slot = _vec_get_ptr(v, idx);
  VecData *data = _vec_data(v);

  size_t size = (data->len - idx) * data->item_size;
  memmove((char *)slot + data->item_size, slot, size);

  memcpy(slot, item, data->item_size);
  data->len++;

  return true;
}

static void _vec_print(Vec *v) {
  VecData *data = _vec_data(v);

  if (data->print == NULL) {
    printf(
        "WARN: Vector does not have 'print' defined. Cannot print vector.\n");
    return;
  }

  printf("[");

  for (size_t i = 0; i < data->len; i++) {
    char *repr = data->print(_vec_get_ptr(v, (int)i));
    printf("%s", repr);
    if (i != data->len - 1) {
      printf(", ");
    }
    free(repr);
  }

  printf("]");
}
