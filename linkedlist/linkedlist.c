#include "linkedlist.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct LLData LLData;
typedef struct LLNode LLNode;

struct LLData {
  LLNode *first;
  LLNode *last;
  size_t len;
  size_t item_size;

  void (*drop_fn)(void *data);
  char *(*print_fn)(void *data);
};

struct LLNode {
  void *data;
  LLNode *next;
};

static bool _is_valid_idx(LL *ll, int idx) {
  LLData *data = ll->_data;

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

static int _translate_idx(LL *ll, int idx) {
  LLData *lld = ll->_data;
  if (idx == -1) {
    return lld->len - 1;
  }
  return idx;
}

static LLNode *_node_new(LLData *lld, void *item) {
  LLNode *node = malloc(sizeof(LLNode));

  if (node == NULL) {
    perror("Memory allocation failed for new node.");
    return NULL;
  }

  node->data = malloc(lld->item_size);

  if (node->data == NULL) {
    perror("Memory allocation failed for new node data.");
    free(node);
    return NULL;
  }

  memcpy(node->data, item, lld->item_size);
  node->next = NULL;

  return node;
}

static void *ll_first(LL *ll);
static void *ll_last(LL *ll);
static void *ll_get(LL *ll, int idx);
static void ll_push(LL *ll, void *item);
static void ll_put(LL *ll, int idx, void *item);
static void *ll_pop(LL *ll);
static void *ll_remove(LL *ll, int idx);
static size_t ll_len(LL *ll);
static int ll_index_of(LL *ll, void *item);
static bool ll_contains(LL *ll, void *item);
static void ll_free(LL *ll);
static void ll_print(LL *ll);

LinkedList linkedlist_new(
  size_t item_size,
  void (*drop_fn)(void *data),
  char *(*print_fn)(void *data)
) {
  LinkedList ll = {0};
  LLData *lld = malloc(sizeof(LLData));

  if (lld == NULL) {
    perror("Memory allocation failed for linked list.");
    return ll;
  }

  lld->first = NULL;
  lld->last = NULL;
  lld->len = 0;
  lld->item_size = item_size;

  lld->drop_fn = drop_fn;
  lld->print_fn = print_fn;

  ll.first = ll_first;
  ll.last = ll_last;
  ll.get = ll_get;
  ll.push = ll_push;
  ll.put = ll_put;
  ll.pop = ll_pop;
  ll.remove = ll_remove;
  ll.len = ll_len;
  ll.index_of = ll_index_of;
  ll.contains = ll_contains;
  ll.free = ll_free;
  ll.print = ll_print;

  ll._data = lld;
  return ll;
}

static void *ll_first(LL *ll) {
  LLData *lld = ll->_data;
  if (lld->first == NULL) {
    return NULL;
  }
  return lld->first->data;
}
static void *ll_last(LL *ll) {
  LLData *lld = ll->_data;
  if (lld->last == NULL) {
    return NULL;
  }
  return lld->last->data;
}

static size_t ll_len(LL *ll) {
  LLData *lld = ll->_data;
  return lld->len;
}

static void *ll_get(LL *ll, int idx) {
  LLData *lld = ll->_data;

  if (!_is_valid_idx(ll, idx)) {
    return NULL;
  }

  idx = _translate_idx(ll, idx);

  LLNode *curr = lld->first;
  for (int i = 0; i < idx; i++) {
    curr = curr->next;
  }

  return curr->data;
}

static void ll_push(LL *ll, void *item) {
  LLData *lld = ll->_data;
  LLNode *new_item = _node_new(lld, item);

  if (new_item == NULL) {
    return;
  }

  if (lld->len == 0) {
    lld->first = new_item;
    lld->last = new_item;
  } else {
    lld->last->next = new_item;
    lld->last = new_item;
  }

  lld->len++;
}

static void ll_put(LL *ll, int idx, void *item) {
  LLData *lld = ll->_data;

  if (idx != 0 && !_is_valid_idx(ll, idx)) {
    return;
  }

  LLNode *new_item = _node_new(lld, item);

  if (new_item == NULL) {
    return;
  }

  if (idx == 0) {
    new_item->next = lld->first;
    lld->first = new_item;

    if (lld->len == 0) {
      lld->last = new_item;
    }

    lld->len++;
    return;
  }

  if (idx == -1) {
    lld->last->next = new_item;
    lld->last = new_item;

    lld->len++;
    return;
  }

  LLNode *curr = lld->first;
  for (int i = 0; i < idx - 1; i++) {
    curr = curr->next;
  }

  new_item->next = curr->next;
  curr->next = new_item;
  lld->len++;
}

static void *ll_pop(LL *ll) {
  LLData *lld = ll->_data;

  if (lld->len == 0) {
    return NULL;
  }

  // allocate mem for popped value.
  void *popped = malloc(lld->item_size);

  if (popped == NULL) {
    perror("Memory allocation failed for popped item.");
    return NULL;
  }

  // copy pointer to ll's last item into popped.
  memcpy(popped, lld->last->data, lld->item_size);

  LLNode *old_last = lld->last;

  if (lld->len == 1) {
    lld->first = NULL;
    lld->last = NULL;
  } else {
    LLNode *new_last = lld->first;

    while (new_last->next != old_last) {
      new_last = new_last->next;
    }

    new_last->next = NULL;
    lld->last = new_last;
  }

  // free the memory for the popped node.
  free(old_last->data);
  free(old_last);
  lld->len--;

  return popped;
}

static void *ll_remove(LL *ll, int idx) {
  LLData *lld = ll->_data;

  if (!_is_valid_idx(ll, idx)) {
    return NULL;
  }

  if (idx == -1) {
    return ll_pop(ll);
  }

  idx = _translate_idx(ll, idx);

  // allocate mem for removed item
  void *removed = malloc(lld->item_size);

  if (removed == NULL) {
    perror("Memory allocation failed for removed item.");
    return NULL;
  }

  LLNode *before = NULL;
  LLNode *curr = lld->first;
  LLNode *after = NULL;

  // walk the list to find the correct node
  for (int i = 0; i < idx; i++) {
    if (i == idx - 1) {
      before = curr;
    }
    curr = curr->next;
  }

  after = curr->next;

  // link the nodes before and after the removed node
  if (before == NULL) {
    lld->first = after;
  } else {
    before->next = after;
  }

  if (curr == lld->last) {
    lld->last = before;
  }

  // copy the data from the removed node to the new item.
  memcpy(removed, curr->data, lld->item_size);

  // free the mem for the removed node.
  free(curr->data);
  free(curr);
  curr = NULL;

  lld->len--;

  return removed;
}

static int ll_index_of(LL *ll, void *item) {
  LLData *lld = ll->_data;

  LLNode *curr = lld->first;
  for (size_t i = 0; i < lld->len; i++) {
    if (memcmp(item, curr->data, lld->item_size) == 0) {
      return i;
    }
    curr = curr->next;
  }

  return -1;
}

static bool ll_contains(LL *ll, void *item) {
  if (ll_index_of(ll, item) != -1) {
    return true;
  }
  return false;
}

static void ll_free(LL *ll) {
  LLData *lld = ll->_data;

  LLNode *curr = lld->first;

  // walk the linked list.
  while (curr != NULL) {

    // if the drop_fn exists use it to free the data held by the node
    if (lld->drop_fn != NULL) {
      lld->drop_fn(curr->data);
    }

    // prepare next node and free the current one.
    LLNode *next = curr->next;
    free(curr->data);
    free(curr);
    curr = next;
  }

  lld->first = NULL;
  lld->last = NULL;
  lld->len = 0;
  lld->item_size = 0;

  free(lld);
  lld = NULL;
}

static void ll_print(LL *ll) {
  LLData *lld = ll->_data;
  if (lld->print_fn == NULL) {
    printf("WARN: print_fn not defined, cannot print linked list.\n");
    return;
  }

  LLNode *curr = lld->first;
  while (curr != NULL) {
    char *str = lld->print_fn(curr->data);
    if (str != NULL) {
      printf("%s", str);
      free(str);
    }

    if (curr->next != NULL) {
      printf(" -> ");
    }

    curr = curr->next;
  }
}
