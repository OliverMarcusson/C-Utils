#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Macros
#define vec_get(v, type, idx) (*(type *)_vec_get((v), (idx)))

#define vec_push_val(v, val)                                                   \
  do {                                                                         \
    __typeof__(val) tmp = (val);                                               \
    vec_push((v), &tmp);                                                       \
  } while (0)

// A vector that stores generic data.
// Owns the values inside it.
// Pointers pushed to a Vec should be heap allocated.
// Any vec with items that need deconstructing needs to define the drop
// function.
typedef struct Vec {
  void *items;
  size_t len;
  size_t cap;
  size_t item_size;
  void (*drop)(void *);
} Vec;

// Returns true if idx is a valid vector index and false if it's not.
// -1 is interpreted as the last item in a vector,
// and is valid.
bool _is_valid_idx(Vec *v, int idx) {
  if (idx >= v->len) {
    return false;
  }

  if (idx < -1) {
    return false;
  }

  return true;
}

int _translate_idx(Vec *v, int idx) {
  if (idx == -1) {
    return v->len - 1;
  }
  return idx;
}

void *_vec_get_item_at(Vec *v, int idx) {
  return (char *)(v->items + v->item_size * idx);
}

// Returns a pointer to shallow copied data, transfering its owenership.
void *_move_data(void *src, size_t len) {
  void *new_item = malloc(len);
  if (new_item == NULL) {
    perror("Memory allocation failed.");
    return NULL;
  }

  memcpy(new_item, src, len);
  return new_item;
}

// Returns a new vector for a specific item size.
// Define drop_fun if items stored in the vec needs destructing on free.
Vec vec_new(size_t item_size, void *drop_fun) {
  Vec v;
  v.items = malloc(item_size * 8);

  if (v.items == NULL) {
    perror("Failed to malloc memory for vec items.");
  }

  v.len = 0;
  v.cap = 8;
  v.item_size = item_size;
  v.drop = NULL;

  if (drop_fun != NULL) {
    v.drop = drop_fun;
  }

  return v;
}

// Frees the vector and deconstructs its data if the vector's drop function is
// defined.
void vec_free(Vec *v) {
  if (v->drop != NULL) {
    for (int i = 0; i < v->len; i++) {
      if (v->items != NULL) {
        v->drop((char *)v->items + i * v->item_size);
      }
    }
  }
  free(v->items);
}

// Pushes the item to the vector, moving its ownership to it.
// If the item is a pointer or contains a pointer, the memory it's pointing at
// should be heap allocated.
bool vec_push(Vec *v, void *item) {
  if (v->len == v->cap) {
    void *new_items = realloc(v->items, v->cap * v->item_size * 2);

    if (new_items == NULL) {
      perror("Failed to realloc memory for vec expansion.");
      return false;
    }

    v->cap *= 2;
    v->items = new_items;
  }

  memcpy((char *)v->items + v->len * v->item_size, item, v->item_size);
  v->len++;
  return true;
}

// Returns a borrowed pointer to an item in the vector. Do not free this.
void *_vec_get(Vec *v, int idx) {
  if (!_is_valid_idx(v, idx)) {
    return NULL;
  }

  idx = _translate_idx(v, idx);

  return (char *)v->items + idx * v->item_size;
}

// Returns a pointer to data moved out of the vector, moving its owenership to
// the caller. This data needs to be freed by the caller.
void *vec_pop(Vec *v, int idx) {
  bool mem_move = _translate_idx(v, idx) == v->len - 1 ? false : true;

  char *item = _vec_get(v, idx);
  void *popped_item = _move_data(item, v->item_size);

  // Zero out the memory where the data used to reside.
  for (int i = 0; i < v->item_size; i++) {
    *(item + i) = '\0';
  }

  if (mem_move) {
    size_t size = v->len * v->item_size - (v->len - 1 - idx) * v->item_size;
    memmove(item, item + v->item_size, size);
  }

  v->len--;

  return popped_item;
}

// Pops the item at index idx into the provided buffer dst, moving it's
// ownership. If data is heap allocated, it needs to be freed by the caller.
void vec_pop_into(Vec *v, void *dst, int idx) {
  void *popped_item = vec_pop(v, idx);
  memcpy(dst, popped_item, v->item_size);
  free(popped_item);
}

typedef struct {
  char *name;
  unsigned age;
} Person;

void person_drop(Person *p) {
  free(p->name);
  p->name = NULL;
}

int main() {
  Vec int_vec = vec_new(sizeof(int), NULL);

  vec_push_val(&int_vec, 4);
  vec_push_val(&int_vec, 5);
  vec_push_val(&int_vec, 6);

  printf("%d\n", vec_get(&int_vec, int, 0));
  printf("%d\n", vec_get(&int_vec, int, 1));
  printf("%d\n", vec_get(&int_vec, int, 2));

  int i;
  vec_pop_into(&int_vec, &i, 2);
  printf("%d\n", i);
  printf("%d", (int)int_vec.len);

  vec_free(&int_vec);

  Vec person_vec = vec_new(sizeof(Person), person_drop);
  Person a;
  a.name = strdup("Oliver");
  a.age = 20;

  vec_push(&person_vec, &a);
  printf("%s", vec_get(&person_vec, Person, 0).name);

  Person *b = vec_pop(&person_vec, 0);
  vec_free(&person_vec);
  return 0;
}
