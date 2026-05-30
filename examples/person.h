#ifndef PERSON_H
#define PERSON_H

// An example struct for the vector implementation.
typedef struct {
  char *name;
  unsigned age;
} Person;

// Frees the data allocated in person.
// Use if the person struct is created on the stack.
void person_drop(void *item);

// Frees the data allocated by a person.
// Use if person was allocated on the heap.
void person_free(Person *p);

// Returns a `char *` representation of the provided person.
// `item` must be castable to `Person *`.
char *person_print(void *item);
#endif
