#include <stdio.h>
#include "../linkedlist/linkedlist.h"
#include <stdlib.h>
#include <string.h>

void drop_str(void *data) {
    char *str = *(char **)data;
    free(str);
}

char *print_str(void *data) {
    char *str = *(char **)data;
    return strdup(str);
}

int main() {
    LinkedList ll = linkedlist_new(sizeof(char *), drop_str, print_str);

    char *a = strdup("hej");
    char *b = strdup("on");
    char *c = strdup("you");

    ll.push(&ll, &a);
    ll.push(&ll, &b);
    ll.push(&ll, &c);

    ll.print(&ll);
    printf("\n");

    char *d = *(char **)ll.pop(&ll);
    printf("%s\n", d);
    free(d);

    ll.print(&ll);
    printf("\n");

    char *e = strdup("123abc");
    ll.put(&ll, 1, &e);

    ll.print(&ll);
    printf("\n");

    printf("%d\n", (int)ll.len(&ll));
    ll.free(&ll);
    return 0;
}
