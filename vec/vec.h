#ifndef VEC_H
#define VEC_H 

#include <stdbool.h>
#include <stddef.h>

// Should return a heap allocated `char *` representation of the provided item.
// The vector's `print` method will handle freeing the memory of this string.
typedef char *(*_VecPrintFn)(void *item);

// Should free the all memory allocated by the provided item,
// regardless if the whole item is heap allocated or only partly heap allocated.
typedef void (*_VecDropFn)(void *item);

typedef struct Vec Vec;

// A vector that stores generic data.
// - Owns the values inside it.
// - Pointers pushed to a Vec should be heap allocated.
// - Any vec with items that need deconstructing needs to define `drop`.
// - Any vec that needs to be able to be printed needs to be define `print`.
struct Vec {
  void *_data;

  void *(*get_ptr)(Vec *v, int idx);
  size_t (*len)(Vec *v);
  size_t (*cap)(Vec *v);
  size_t (*item_size)(Vec *v);
  void (*free)(Vec *v);
  bool (*push)(Vec *v, void *item);
  void *(*pop)(Vec *v, int idx);
  void (*pop_into)(Vec *v, void *dst, int idx);
  bool (*insert)(Vec *v, void *item, int idx);
  void (*print)(Vec *v);
};

// Returns a new vector for a specific item size.
// Define `drop_fun` if items stored in the vec needs to be deconstructed on free.
// Must be freed with the vector's `free` method.
Vec vec_new(size_t item_size, _VecDropFn drop, _VecPrintFn print);

// Returns a borrowed, dereferenced item from the specified index.
// Only use when item can't be NULL, like when the item is an `int`;
// #define vec_get_as(v, type, idx) (*(type *)(v)->get_ptr((v), (idx)))

// Returns a borrowed pointer to the data at the specified index.
#define vec_get_ptr_as(v, type, idx) (type *)(v)->get_ptr((v), (idx))

// Pushes a value to the vector by temporarily allocating it on the stack.
#define vec_push_val(v, val)                                                   \
  do {                                                                         \
    __typeof__(val) tmp = (val);                                               \
    (v)->push((v), &tmp);                                                      \
  } while (0)

#endif
