#include "headers.h"
#include "sock.h"
#include "classes.h"
#include "color.h"
#include "getconf.h"

// struct UNode *users = NULL;
struct UNode *me = NULL;
char my_name[20];

int main(int argc, char **argv) {
    int port, listener, epoll_fd, new_fd, sub_fd;
    pthread_t stid, btid, rtid, intid; // 发送， 心跳， 接收， 初始化

    // 1. process arguments
    if (argc == 1) {
        port = atoi(get_value(CONF_PATH, "port"));
        strncpy(my_name, get_value(CONF_PATH, "name"), sizeof(my_name));
    }
    else if (argc == 3) {
        port = atoi(argv[2]);
        strcpy(my_name, argv[1]);
    }
    else {
        fprintf(stderr, "use %s name port, or %s\n", argv[0], argv[0]);
        return 0;
    }

    fprintf(stderr, GRAY "uname = %s, port = %d\n" NOCOLOR, my_name, port);

    // 2. start listener

    if ((listener = sock_udp(port)) < 0) {
        perror("sock_udp");
        return 0;
    }

    // 3. epoll

    if ((epoll_fd = epoll_create(1)) < 0) {
        perror("epoll_create epoll_fd");
        return 0;
    }


    if ((sub_fd = epoll_create(1)) < 0) {
        perror("epoll_create sub_fd");
        return 0;
    }

    struct epoll_event ev, events[MAX_EVENTS];

    ev.events = EPOLLIN;
    ev.data.fd = listener;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        perror("listener epoll_ctl");
        return 0;
    }

    // 4. receiving 

    me = init_list(my_name, "0", listener);


    // pthread_create(&stid, NULL, sender_handler, me);
    pthread_create(&rtid, NULL, ht_handler, me);
    pthread_create(&intid, NULL, init_handler, &listener);
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        if (nfds < 0) {
            perror("epoll_wait");
            return 0;
        }


        for (int i = 0; i < nfds; i += 1) {
            
            struct UNode *new_user = (struct UNode *) malloc(sizeof(struct UNode)), *check_node;
            if ((new_fd = udp_accept(listener, new_user)) < 0) {
                fprintf(stderr, RED "udp accept error\n");
                continue;
            }

            check_node = find_node(new_user -> ip, new_user -> port, me);

            if (check_node == NULL) {
                fprintf(stderr, GREEN "\t in main " NOCOLOR "find new user %s\n", new_user -> uname);
                add_to_reactor(sub_fd, new_user);
                push_list(new_user, me);
            }
            else {
                fprintf(stderr, GREEN "\t in main " NOCOLOR "already had user %s\n", new_user -> uname);
                check_node -> online = FL_ONLINE;
                free(new_user);
            }

            if (new_fd == CHAT_FIN) {
                drop_node(check_node, me);
            }
            

        }

    }

    return 0;
}
