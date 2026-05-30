#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "person.h"

// Frees the data allocated in person.
// Use if the person struct is created on the stack.
void person_drop(void *item) {
  Person *p = item;
  if (p == NULL) {
    return;
  }
  
  free(p->name);
  p->name = NULL;
  p->age = 0;
}

// Frees the data allocated by a person.
// Use if person was allocated on the heap.
void person_free(Person *p) {
  if (p == NULL) {
    return;
  }
  
  person_drop(p);
  free(p);
}

char *person_print(void *item) {
  Person *p = item;
  int needed = snprintf(NULL, 0, "Person: %s, %d", p->name, p->age);
  if (needed < 0) {
    return NULL;
  }
  
  char *string = (char *)malloc((size_t)needed + 1);
  if (string == NULL)  {
    perror("Memory allocation failed");
    return NULL;
  }

  snprintf(string, (size_t)needed + 1, "Person: %s, %d", p->name, p->age);
  return string;
}
