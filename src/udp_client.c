#include "headers.h"
#include "sock.h"

int client_udp() {
    int sfd;

    if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    return sfd;
}
