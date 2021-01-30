#include "headers.h"
#include "classes.h"
#include "sock.h"
#include "color.h"

extern struct UNode *me;

int sock_udp(int port) {
    int listener;
    if ((listener = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    unsigned long opt = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        return -1;
    }

    return listener;
}


int client_udp() {
    int sfd;

    if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    return sfd;
}


int udp_accept(int fd, struct UNode *user) {
    int new_fd, ret;
    struct sockaddr_in client;
    struct Message recv_msg, reply;

    bzero(&recv_msg, sizeof(recv_msg));
    bzero(&reply, sizeof(reply));

    socklen_t clen = sizeof(client);

    ret = recvfrom(fd, (void *) &recv_msg, sizeof(recv_msg), 0, (struct sockaddr *) &client, &clen);

    if (ret != sizeof(recv_msg)) {
        reply.type = CHAT_FIN;
        strcpy(reply.payload, "log in request error");
        sendto(fd, (void *) &reply, sizeof(reply), 0, (struct sockaddr *) &client, clen);

        fprintf(stderr, RED "accept package not complete\n");
        return -1;
    }

    strcpy(user -> uname, recv_msg.from);
    user -> online = FL_ONLINE;
    inet_ntop(AF_INET, &client.sin_addr, user -> ip, sizeof(client));
    user -> port = ntohs(client.sin_port);
    if ((user -> fd = udp_connect(&client)) < 0) {
        perror("udp_connect");
        return -1;
    }

    if (recv_msg.type & CHAT_ACK == 0 && recv_msg.type & CHAT_FIN == 0) {
        reply.type = CHAT_ACK | recv_msg.type;
        sprintf(reply.payload, "ncc accept a msg with %d from you...", recv_msg.type);
        strcpy(reply.from, me -> uname);
        send(user -> fd, &reply, sizeof(reply), 0);
    }

    fprintf(stderr, GREEN "accept %d > " GRAY " %s, %s:%d with " NOCOLOR "%s\n", 
        recv_msg.type, recv_msg.from, user -> ip, user -> port, recv_msg.payload);
    return recv_msg.type;
}


int udp_connect(struct sockaddr_in *addr) {
    int sock_fd;
    if ((sock_fd = sock_udp(PORT)) < 0) {
        return -1;
    }

    if (connect(sock_fd, (struct sockaddr *) addr, sizeof(struct sockaddr)) < 0) {
        return -1;
    }

    return sock_fd;
}


void del_from_reactor(int sub_fd, struct UNode *del_user) {
    epoll_ctl(sub_fd, EPOLL_CTL_DEL, del_user -> fd, NULL);
    return ;
}


// void add_event_ptr(int sub_fd, int fd, int events, struct UNode *user) {
//     struct epoll_event ev;
//     ev.events = events;
//     ev.data.ptr = (void *) user;

//     epoll_ctl(sub_fd, EPOLL_CTL_ADD, fd, &ev);
//     return ;
// }


int add_to_reactor(int sub_fd, struct UNode *new_user) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = (void *) new_user;
    epoll_ctl(sub_fd, EPOLL_CTL_ADD, new_user -> fd, &ev);

    // add_event_ptr(sub_fd, new_user -> fd, EPOLLIN | EPOLLET, new_user);
    return 0;
}
