#include "vec/vec.h"
#include "examples/person.h"
#include <stdio.h>
#include <string.h>

int main() {
    Vec v = vec_new(sizeof(Person), person_drop, person_print);
    vec_push_val(&v, 
        ((Person){
            .name = strdup("Oliver"), 
            .age = 20
        })
    );

    vec_push_val(&v, 
        ((Person){
            .name = strdup("Edvin"), 
            .age = 20
        })
    );

    vec_push_val(&v, 
        ((Person){
            .name = strdup("Agaton"), 
            .age = 22
        })
    );

    v.print(&v);
    printf("\n");

    Person *oliver = v.pop(&v, 0);
    person_free(oliver);

    v.print(&v);
    printf("\n");

    Person *edvin = vec_get_ptr_as(&v, Person, 0);
    printf("%s\n", edvin->name);

    Vec int_v = vec_new(sizeof(int), NULL, NULL);
    vec_push_val(&int_v, 1);
    vec_push_val(&int_v, 2);
    vec_push_val(&int_v, 3);
    printf("%d\n", *(int *)int_v.get_ptr(&int_v, 1));

    v.free(&v),
    int_v.free(&int_v);

    return 0;
}
