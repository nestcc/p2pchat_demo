#include "headers.h"
#include "classes.h"
#include "color.h"
#include "sock.h"
#include "getconf.h"

extern char my_name[20];
extern struct UNode *me;
extern int recv_fd;
// 用于初始化用户表

void *init_handler(void *argv) {
    fprintf(stdout, BLUE "Initializing in 3 seconds..." NOCOLOR "\n");
    sleep(1);
    FILE *ip_list = NULL;
    // if ((ip_list = fopen(IPLIST_PATH, "r")) == NULL) {
    //     fprintf(stdout, RED "open ip file ( %s ) error" NOCOLOR "\n", IPLIST_PATH);
    //     exit(EXIT_FAILURE);
    //     return NULL;
    // }

    size_t nread, lsize;
    int init_fd = me -> fd;
    char *lcontent = NULL;

    while (1) {
        fprintf(stderr, GREEN "init thread working\n");
        if ((ip_list = fopen(IPLIST_PATH, "r")) == NULL) {
            fprintf(stdout, RED "open ip file ( %s ) error" NOCOLOR "\n", IPLIST_PATH);
            exit(EXIT_FAILURE);
            return NULL;
        }
        while ((nread = getline(&lcontent, &lsize, ip_list)) != -1) {
            if (strlen(lcontent) == 0) { continue; }

            // printf( GREEN "SYN %s" NOCOLOR , lcontent);
            struct sockaddr_in client;

            client.sin_family = AF_INET;
            client.sin_port = htons(PORT);
            client.sin_addr.s_addr = inet_addr(lcontent);

            struct Message syn_msg;
            strcpy(syn_msg.from, my_name);
            syn_msg.type = CHAT_SYN;

            // fprintf(stderr, GREEN "init > " NOCOLOR "%s" , lcontent);
            sendto(init_fd, &syn_msg, sizeof(syn_msg), 0, (struct sockaddr *) &client, sizeof(client));
        }

        free(lcontent);
        fclose(ip_list);

        sleep(300);
    }

    return NULL;
}


// 用于发送消息
void *sender_handler(void *argv) {
    sleep(3);
    fprintf(stderr, GRAY "sender ok\n");
    // struct sockaddr_in peer;
    // peer.sin_family = AF_INET;
    // peer.sin_port = htons(PORT);

    while (1) {
        struct Message new_msg;
        bzero(&new_msg, sizeof(new_msg));
        printf(BLUE "message > " NOCOLOR);
        scanf("%s", new_msg.payload);
        strcpy(new_msg.from, me -> uname);
        new_msg.type = CHAT_SYN;

        if (me == NULL) {
            fprintf(stderr, RED "head is NULL\n" NOCOLOR);
            exit(1);
        }
        struct UNode *tmp = me -> next;
        while (tmp != me) {
            if (tmp -> online <= 0) {
                tmp = tmp -> next;
                continue;
            }
            send(tmp -> fd, &new_msg, sizeof(new_msg), 0);
            // peer.sin_port = htons(tmp -> port);
            // peer.sin_addr.s_addr = inet_addr(tmp -> ip);
            // sendto(me -> fd, &new_msg, sizeof(new_msg), 0, (struct sockaddr *) &peer, sizeof(peer));
            tmp = tmp -> next;
        }
    }

    return NULL;
}

// 用于接收消息
void *receiver_handler(void *argv) {
    sleep(3);
    int fd = *(int *) argv, nfds;
    struct Message recv_msg, reply;
    struct epoll_event events[100];

    while (1) {
        nfds = epoll_wait(recv_fd, events, 100, -1);
        fprintf(stderr, GREEN "subfd > nfds = %d\n", nfds);
        if (nfds < 0) { exit(EXIT_FAILURE); }

        for (int i = 0; i < nfds; i += 1) {
            struct UNode *curr_u = (struct UNode *) (events[i].data.ptr);
            recv(curr_u -> fd, &recv_msg, sizeof(recv_msg), 0);

            if (recv_msg.type & CHAT_ACK) {
                fprintf(stderr, GREEN "accept %d > " GRAY " %s, %s:%d with " NOCOLOR "%s\n",
                    recv_msg.type, recv_msg.from, curr_u -> ip, curr_u -> port, recv_msg.payload);
                continue ;
            }
            else if (recv_msg.type & CHAT_FIN) {
                fprintf(stderr, GREEN "accept %d > " GRAY " %s, %s : %d with " NOCOLOR "%s, so log out...\n",
                    recv_msg.type, recv_msg.from, curr_u -> ip, curr_u -> port, recv_msg.payload);
                del_from_reactor(recv_fd, curr_u);
                drop_node(curr_u, me);
                continue ;
            }
            else if (recv_msg.type & CHAT_HRT) {
                reply.type = CHAT_ACK | CHAT_HRT;
                strcpy(reply.payload, "ncc got your HRT, I am still online...\n");
            }
            else if (recv_msg.type & CHAT_SYN) {
                reply.type = CHAT_ACK | CHAT_SYN;
                strcpy(reply.payload, "ncc got your SYN, good luck!\n");
            }
            else {
                reply.type = CHAT_FIN;
                sprintf(reply.payload, "ncc cannot accept a msg with <%d> from you, what is that?", recv_msg.type);
            }

            strcpy(reply.from, me -> uname);
            send(curr_u -> fd, &reply, sizeof(reply), 0);

            fprintf(stderr, GREEN "subfd %d > " GRAY " %s, %s:%d with " NOCOLOR "%s\n",
                recv_msg.type, recv_msg.from, curr_u -> ip, curr_u -> port, recv_msg.payload);
        }

    }

    return NULL;
}

void *ht_handler(void *argv) {
    sleep(3);
    if (argv == NULL) {
        fprintf(stderr, RED "in ht_handler > " NOCOLOR "argv is NULL\n");
        exit(EXIT_FAILURE);
    }

    struct UNode *tmp;

    struct Message ht_msg;
    bzero(&ht_msg, sizeof(ht_msg));
    strcpy(ht_msg.from, me -> uname);
    strcpy(ht_msg.payload, "A heartbeat message to you, good luck!");
    ht_msg.type = CHAT_HRT;

    struct sockaddr_in peer;
    peer.sin_family = AF_INET;

    fprintf(stderr, GRAY "HT ok\n");
    while (1) {
        sleep(30);
        fprintf(stderr, GREEN "sending ht \n");
        tmp = me -> next;
        while (tmp != me) {
            if (tmp -> online <= 0) {
                tmp = tmp -> next;
                drop_node(tmp -> prev, me);
                continue;
            }
            // peer.sin_port = htons(tmp -> port);
            // peer.sin_addr.s_addr = inet_addr(tmp -> ip);
            // sendto(me -> fd, &ht_msg, sizeof(ht_msg), 0, (struct sockaddr *)&peer, sizeof(peer));
            send(tmp -> fd, &ht_msg, sizeof(ht_msg), 0);
            tmp -> online -= 1;
            fprintf(stderr, GREEN "\tht to " NOCOLOR "%s %d\n", tmp -> uname, tmp -> online);
            tmp = tmp -> next;
        }
    }
    return NULL;
}

// 忘了是要干什么的了
void *mtsend_handler(void *argv) {

    return NULL;
}

