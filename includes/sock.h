#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int sock_udp(int port);
int client_udp();
int udp_accept(int fd, struct UNode *user);
int add_to_reactor(int sub_fd, struct UNode *new_user);
void del_from_reactor(int sub_fd, struct UNode *del_user);
int udp_connect(struct sockaddr_in *addr);

void *init_handler(void *argv);
void *sender_handler(void *argv);
void *receiver_handler(void *argv);
void *ht_handler(void *argv);
struct UNode *login_user(struct UNode *user, struct UNode *ugroup);
