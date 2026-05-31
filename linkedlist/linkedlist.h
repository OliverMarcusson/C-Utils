#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct LinkedList LinkedList;
typedef LinkedList LL;

struct LinkedList {
  void *_data;

  // Returns a borrowed pointer to the first item in the list.
  void *(*first)(LL *ll);

  // Returns a borrowed pointer to the last item in the list.
  void *(*last)(LL *ll);

  // Returns a borrowed pointer to the item at the specified index.
  void *(*get)(LL *ll, int idx);

  // Pushes the provided item to the end of the list.
  // Moves ownership of the item to the linked list.
  void (*push)(LL *ll, void *item);

  // Puts the provided item at the specified index in the list.
  // Moves ownership of the item to the linked list.
  void (*put)(LL *ll, int idx, void *item);

  // Pops the last item off the list and returns it.
  // Moves ownership of the item to the caller.
  void *(*pop)(LL *ll);

  // Returns the length of the list.
  size_t (*len)(LL *ll);

  // Returns which index the specified item resides at.
  // Returns `-1` if the item is not found in the list.
  int (*index_of)(LL *ll, void *item);

  // Returns `true` if the specified item exists in the list.
  // Returns `false` if it does not.
  bool (*contains)(LL *ll, void *item);
};

#endif // !LINKEDLIST_H
