#include "headers.h"
#include "sock.h"
#include "classes.h"
#include "color.h"


int sock_udp(int port) {
    int listener, opt;
    if ((listener = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    // setsockopt(listener, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

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

    // if ((recv_msg.type & CHAT_SYN) && (recv_msg.type & CHAT_ACK)) {

    // }
    // if (recv_msg.type != CHAT_SYN) {

        
    //     reply.type = CHAT_FIN;
    //     strcpy(reply.payload, "error SYN");
    //     sendto(fd, (void *) &reply, sizeof(reply), 0, (struct sockaddr *) &client, clen);
    //     return -1;
    // }

    // fprintf(stderr, GREEN "accept > " NOCOLOR " %s with %s\n", recv_msg.from, recv_msg.payload);

    strcpy(user -> uname, recv_msg.from);
    user -> online = FL_ONLINE;
    inet_ntop(AF_INET, &client.sin_addr, user -> ip, sizeof(client));
    user -> port = ntohs(client.sin_port);
    // if ((new_fd = udp_connect(&client)) < 0) {
    //     // free(user);
    //     perror("udp_connect");
    //     return -1;
    // }

    // user -> fd = new_fd;

    // char buff[512] = { 0 };


    if (recv_msg.type == CHAT_SYN) {
        reply.type = CHAT_ACK;
        sprintf(reply.payload, "Greating, ncc has accepted you %s", user -> uname);
    }
    else if (recv_msg.type == CHAT_HRT) {
        reply.type = CHAT_ACK;
        strcpy(reply.payload, "ncc got your HRT, I am still online...\n");
    }
    else if (recv_msg.type == CHAT_FIN || recv_msg.type == CHAT_ACK) {
        fprintf(stderr, GREEN "accept %d > " GRAY " %s, %s with " NOCOLOR "%s\n", 
            recv_msg.type, recv_msg.from, user -> ip, recv_msg.payload);
        return recv_msg.type;
    }
    // else if (recv_msg.type != CHAT_ACK) {
    //     reply.type = CHAT_FIN;
    //     sprintf(reply.payload, "ncc cannot accept a msg with %d from you, what is that?", recv_msg.type);
    // }
    else {
        reply.type = CHAT_FIN;
        sprintf(reply.payload, "ncc cannot accept a msg with %d from you, what is that?", recv_msg.type);
    }

    strcpy(reply.from, "ncc");
    sendto(fd, (void *) &reply, sizeof(reply), 0, (struct sockaddr *)&client, sizeof(client));

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


void add_event_ptr(int sub_fd, int fd, int events, struct UNode *user) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = (void *) user;

    epoll_ctl(sub_fd, EPOLL_CTL_ADD, fd, &ev);
}


int add_to_reactor(int sub_fd, struct UNode *new_user) {
    // fprintf(stderr, GREEN "\taccept %s\n", new_user -> uname);
    int sub = new_user -> fd;
    // new_user[sub] = *new_user;
    add_event_ptr(sub_fd, new_user -> fd, EPOLLIN | EPOLLET, new_user);
}


// struct UNode *login_user(struct UNode *user, struct UNode *ugroup) {
//     // if (user -> fd >= MAX_USER) {
//     //     fprintf(stderr, RED "Too many connections\n" NOCOLOR);
//     //     return ;
//     // }

//     // user -> online = 1;
//     // memcpy(&ugroup[user -> fd], user, sizeof(user));

//     ugroup = push_list(user -> uname, user -> ip, user -> fd, ugroup);
//     return ugroup;
// }
