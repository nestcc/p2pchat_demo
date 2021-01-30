#include "headers.h"
#include "sock.h"
#include "color.h"
#include "classes.h"

struct conn_info {
    int my_port, sd_port;
    char ip[20];
    char name[20];
};



void *receiver(void *arg) {

    struct conn_info *info = (struct conn_info *) arg;
    int udpfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(info -> my_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(udpfd, (struct sockaddr *) &addr, sizeof(addr))) < 0) {
        perror("receiver bind");
        return NULL;
    }

    char buff[256];

    struct sockaddr_in conn;
    socklen_t clen = sizeof(conn);
    int recv_size;


    while (1) {
        // printf("recving\n");

        struct PkgHeader msg;
        recv_size = 0;
        recv_size += recvfrom(udpfd, &msg, sizeof(msg), 0, (struct sockaddr *) &conn, &clen);
        recv_size += recvfrom(udpfd, buff, msg.length, 0, (struct sockaddr *) &conn, &clen);


        if (recv_size <= 0) {
            printf( RED "recv with size 0\n");
        }

        printf("\n" GREEN "%s > " NOCOLOR "%s\n\n", msg.name, buff);
        printf( BLUE "send > ");
    }

    return NULL;
}

void *sender(void *arg) {
    struct conn_info *info = (struct conn_info *) arg;
    int udpfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(info -> sd_port);
    addr.sin_addr.s_addr = inet_addr(info -> ip);

    char buff[256];

    int send_size;

    while (1) {

        struct PkgHeader msg;
        strcpy(msg.name, info -> name);

        printf( BLUE "send > ");
        scanf("%s", buff);

        msg.length = strlen(buff);
        send_size = 0;

        send_size += sendto(udpfd, &msg, sizeof(msg), 0, (struct sockaddr *) &addr, sizeof(addr));
        send_size += sendto(udpfd, buff, strlen(buff), 0, (struct sockaddr *) &addr, sizeof(addr));

        if (send_size <= 0) {
            printf( RED "send with size 0\n");
        }

    }

    return NULL;
}



int main(int argc, char **argv) {

    if (argc != 5) {
        fprintf(stderr, RED "use %s name ip my_port send_port\n", argv[0]);
    }
    struct conn_info cn_info;

    bzero(&cn_info, sizeof(cn_info));
    strcpy(cn_info.name, argv[1]);
    strcpy(cn_info.ip, argv[2]);
    cn_info.my_port = atoi(argv[3]);
    cn_info.sd_port = atoi(argv[4]);

    pthread_t stid, rtid;

    pthread_create(&rtid, NULL, receiver, (void *) &cn_info);

    usleep(1000);
    pthread_create(&stid, NULL, sender, (void *) &cn_info);

    pthread_join(stid, NULL);
    pthread_join(rtid, NULL);


    return 0;
}

