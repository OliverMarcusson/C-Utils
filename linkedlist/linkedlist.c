#include "linkedlist.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct LLData LLData;
typedef struct LLNode LLNode;

struct LLData {
  LLNode *first;
  LLNode *last;
  size_t len;
  size_t item_size;
};

struct LLNode {
  void *data;
  size_t size;
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

void *ll_first(LL *ll);
void *ll_last(LL *ll);
void *ll_get(LL *ll, int idx);
void ll_push(LL *ll, void *item);
void ll_put(LL *ll, int idx, void *item);
void *ll_pop(LL *ll);
size_t ll_len(LL *ll);
int ll_index_of(LL *ll, void *item);
bool ll_contains(LL *ll, void *item);

LinkedList linkedlist_new(size_t item_size) {
  LinkedList ll;
  LLData *lld = malloc(sizeof(LLData));

  lld->first = NULL;
  lld->last = NULL;
  lld->len = 0;
  lld->item_size = item_size;

  ll.first = ll_first;
  ll.last = ll_last;
  ll.get = ll_get;
  ll.push = ll_push;
  ll.put = ll_put;
  ll.pop = ll_pop;
  ll.len = ll_len;
  ll.index_of = ll_index_of;
  ll.contains = ll_contains;

  ll._data = lld;
  return ll;
}

void *ll_first(LL *ll) {
  LLData *lld = ll->_data;
  return lld->first;
}
void *ll_last(LL *ll) {
  LLData *lld = ll->_data;
  return lld->last;
}

size_t ll_len(LL *ll) {
  LLData *lld = ll->_data;
  return lld->len;
}

void *ll_get(LL *ll, int idx) {
  LLData *lld = ll->_data;

  if (!_is_valid_idx(ll, idx)) {
    return NULL;
  }

  idx = _translate_idx(ll, idx);

  LLNode *curr = lld->first;
  for (int i = 0; i < idx; i++) {
    curr = curr->next;
  }

  return curr;
}
