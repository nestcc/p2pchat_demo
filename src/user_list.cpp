#include "headers.h"
#include "sock.h"
#include "classes.h"
#include "color.h"

struct UNode *init_list(const char *in_name, const char *in_ip, int in_fd) {
    struct UNode *new_head = (struct UNode *) malloc(sizeof(struct UNode));
    new_head -> prev = new_head;
    new_head -> next = new_head;
    strcpy(new_head -> uname, in_name);
    strcpy(new_head -> ip, in_ip);
    new_head -> ht_cnt = 0;
    new_head -> online = FL_ONLINE;
    new_head -> fd = in_fd;
    new_head -> ht_cnt = 0;
    new_head -> online = 1;

    return new_head;
}


// struct UNode *push_list(const char *in_name, const char *in_ip, int in_fd, struct UNode *head) {
//     if (head == NULL) {
//         return __init_list(in_name, in_ip, in_fd);
//     }

//     struct UNode *new_node = (struct UNode *) malloc(sizeof(struct UNode));
//     struct UNode *hprev = head -> prev;
//     strcpy(new_node -> uname, in_name);
//     strcpy(new_node -> ip, in_ip);
//     new_node -> ht_cnt = 0;
//     new_node -> online = FL_ONLINE;
//     new_node -> fd = in_fd;

//     new_node -> prev = head -> prev;
//     new_node -> next = head;
//     new_node -> prev -> next = new_node;
//     head -> prev = new_node;

//     return head;
// }

struct UNode *push_list(struct UNode *user, struct UNode *head) {
    if (user == NULL) {
        return head;
    }
    if (head == NULL) {
        return init_list(user -> uname, user -> ip, user -> fd);
    }

    // fprintf(stderr, GREEN "\t in push_list" NOCOLOR " %s\n", user -> uname);
    
    struct UNode *hprev = head -> prev;
    user -> ht_cnt = 0;
    user -> online = FL_ONLINE;

    user -> prev = head -> prev;
    user -> next = head;
    user -> prev -> next = user;
    head -> prev = user;

    return head;
}


int __prt_node(struct UNode *node, struct UNode *head) {
    if (node != head) {
        printf(" -> %s\n", node -> uname);
        __prt_node(node -> next, head);
    }
    return 0;
}


int print_list(struct UNode *head) {
    if (head == NULL) {
        printf("printing NULL\n");
        return 0;
    }
    printf("%s\n", head -> uname);
    __prt_node(head -> next, head);
    return 0;
}


struct UNode *drop_node(struct UNode *rm_node, struct UNode *head) {
    if (rm_node == NULL) {
        printf("removing NULL\n");
        return head;
    }
    if (rm_node == head) {
        head = head -> next;
        if (head -> next == head) {
            free(head);
            return NULL;
        }
    }

    // fprintf(stderr, GRAY "rm %s %s:%d from list" NOCOLOR, rm_node -> uname, rm_node -> ip, rm_node -> port);

    rm_node -> next -> prev = rm_node -> prev;
    rm_node -> prev -> next = rm_node -> next;
    // close(rm_node -> fd);

    rm_node -> next = rm_node -> prev = NULL;
    // fprintf(stderr, "   will drop\n");
    free(rm_node);
    
    return head;
}


struct UNode *find_node(const char *uip, int uport, struct UNode *head) {
    if (head == NULL) { return NULL; }
    struct UNode *tmp = head;

    while (strcmp(tmp -> ip, uip) != 0 || tmp -> port != uport) {
        tmp = tmp -> next;
        if (tmp == head) {
            return NULL;
        }
    }
    return tmp;
}


void traverse_list(struct UNode *head, void (*callback)(void *arg1, void *arg2, void *arg3)) {
    if (head == NULL) {
        printf("traverse list while head is NULL\n");
        return ;
    }
    callback(head, NULL, NULL);

    struct UNode *tmp = head -> next;
    while (tmp != head) {
        callback(tmp, NULL, NULL);
        tmp = tmp -> next;
    }

    return ;
}
