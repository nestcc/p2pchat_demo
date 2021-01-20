#include "headers.h"
#include "classes.h"
#include "color.h"
#include "sock.h"
#include "getconf.h"

extern char my_name[20];
extern struct UNode *me;

// 用于初始化用户表

void *init_handler(void *argv) {
    fprintf(stdout, BLUE "Initializing in 3 seconds..." NOCOLOR "\n");
    sleep(1);
    FILE *ip_list = NULL;
    if ((ip_list = fopen(IPLIST_PATH, "r")) == NULL) {
        fprintf(stdout, RED "open ip file ( %s ) error" NOCOLOR "\n", IPLIST_PATH);
        exit(EXIT_FAILURE);
        return NULL;
    }

    size_t nread, lsize;
    int init_fd = *(int *) argv;
    char *lcontent = NULL;

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

        // if ((init_fd = sock_udp(PORT)) < 0) {
        //     perror("init sock_udp");
        //     exit(EXIT_FAILURE);
        // };

        sendto(init_fd, &syn_msg, sizeof(syn_msg), 0, (struct sockaddr *) &client, sizeof(client));
    }
    // close(init_fd);

    free(lcontent);
    fclose(ip_list);

    return NULL;
}


// 用于发送消息
void *sender_handler(void *argv) {
    sleep(3);
    // struct UNode *me = (struct UNode *) argv;
    fprintf(stderr, GRAY "sender ok\n");
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    // peer.sin_port = htons(PORT);

    while (1) {
        // char send_msg[1024] = { 0 };
        struct Message new_msg;
        bzero(&new_msg, sizeof(new_msg));
        printf(BLUE "message > " NOCOLOR);
        scanf("%s", new_msg.payload);
        strcpy(new_msg.from, me -> uname);
        new_msg.type = CHAT_SYN;

        // send(me -> fd, &new_msg, sizeof(new_msg), 0);

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
            peer.sin_port = htons(tmp -> port);
            peer.sin_addr.s_addr = inet_addr(tmp -> ip);
            sendto(me -> fd, &new_msg, sizeof(new_msg), 0, (struct sockaddr *)&peer, sizeof(peer));
            tmp = tmp -> next;
        }
    }

    return NULL;
}



// // 用于接收消息
// void *receiver_handler(void *argv) {
//     sleep(3);
//     int fd = *(int *) argv;

//     char msg_buff[1024] = { 0 };

//     while (1) {
//         // epoll_wait(fd, )

//     }

//     return NULL;
// }

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
    strcpy(ht_msg.payload, "a heartbeat message to you, best wishes !");
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
            peer.sin_port = htons(tmp -> port);
            peer.sin_addr.s_addr = inet_addr(tmp -> ip);
            sendto(me -> fd, &ht_msg, sizeof(ht_msg), 0, (struct sockaddr *)&peer, sizeof(peer));
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

