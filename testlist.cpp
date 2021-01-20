#include "headers.h"
#include "nccshell.h"
#include "classes.h"

struct AddrNode *list_head = NULL;

NEW_COMMAND(add) {
    char one_name[20];
    scanf("%s", one_name);
    list_head = push_list(one_name, NULL, list_head);
    print_list(list_head);
}

NEW_COMMAND(del) {
    char one_name[20];
    scanf("%s", one_name);

    struct AddrNode *node = find_node(one_name, list_head);
    list_head = drop_node(node, list_head);
    print_list(list_head);
}


NEW_COMMAND(prt) {
    print_list(list_head);
}

void print_node(void *arg1, void *arg2, void *arg3) {
    struct AddrNode *node = (struct AddrNode *) arg1;
    struct AddrNode *l_head = (struct AddrNode *)arg2;

    if (strcmp(node -> name, "abc") == 0) {
        l_head = drop_node(node, l_head);
    }
    printf(" -> %s\n", node -> name);
    return ;
}


NEW_COMMAND(tra) {
    traverse_list(list_head, &print_node);
}

int main() {
    SET_CMD_HEAD("list");
    RUN_SHELL();
}
