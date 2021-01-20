#define EXIT break
typedef void (*ShellFunc)();

typedef struct LinkedNode {         // 链表框架
    struct LinkedNode *next;
} LinkedNode;

typedef struct FuncList {           // 函数链表
    LinkedNode node;
    ShellFunc func;
    char *cmd;
} FuncList;


void regist_command(ShellFunc, const char *);
void RUN_SHELL();

void SET_CMD_HEAD(const char *);
void SET_EXIT_CMD(const char *);


#define NEW_COMMAND(CMD)                                \
void __COMMAND_##CMD ();                                \
__attribute__((constructor)) void __REGIST_##CMD() {    \
    regist_command(__COMMAND_##CMD, #CMD);              \
}                                                       \
void __COMMAND_##CMD ()
