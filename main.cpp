#include "headers.h"
#include "classes.h"

int main() {
    struct PkgHeader *new_msg;

    new_msg = (struct PkgHeader *) malloc(sizeof(struct PkgHeader));

    new_msg -> type = 1;
    new_msg -> length = 150;
    // new_msg -> payload = (char *) malloc(sizeof(new_msg -> length));
    

    printf("void * size %lu\n", sizeof(void *));
    printf("int size = %lu\n", sizeof(int));

    printf("\nsize = %lu\n", sizeof(struct PkgHeader));
    printf("name size = %lu\n", sizeof(new_msg -> name));


    printf("name offset = %ld\n", (char *)(&new_msg -> name) - (char *)(new_msg));
    // printf("payload offset = %ld\n", (char *)(&new_msg -> payload) - (char *)(new_msg));
    return 0;
}
