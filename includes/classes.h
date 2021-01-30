// #define FL_SYNSENT 0x001
#define FL_ONLINE 0x003

struct UNode {
    struct UNode *next, *prev;
    char uname[20];
    char ip[20];
    int port;
    int online;
    int fd;
    int ht_cnt;
};


struct User {
    char uname[20];
    char ip[20];
    // int fd;
    // int online;
};


struct PkgHeader {
    int type;
    int length;
    char name[20];
    // char *payload;
};

#define CHAT_SYN 0x001
#define CHAT_ACK 0x002
#define CHAT_FIN 0x004
#define CHAT_HRT 0x008
#define CHAT_MSG 0x016
#define CHAT_PRI 0x032
#define CHAT_SYS 0x064


struct Message {
    int type;
    char from[20];
    char to[20];
    char payload[1024];
};

struct UNode *init_list(const char *in_name, const char *in_ip, int in_fd);
struct UNode *push_list(struct UNode *user, struct UNode *head);
struct UNode *drop_node(struct UNode *rm_node, struct UNode *head);
struct UNode *find_node(const char *name, int uport, struct UNode *head);
int print_list(struct UNode *head);
void traverse_list(struct UNode *head, void (*callback)(void *arg1, void *arg2, void *arg3));
