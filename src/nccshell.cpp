#include "nccshell.h"
#include "headers.h"

#define HT_SIZE 16

// FuncList ncc_shell_head, *curr = &ncc_shell_head;
FuncList *funcs_htable[HT_SIZE * 2];
int have_cmd;
char *cmd_head = strdup("shell");
char *exit_cmd = strdup("exit");

int BKDR_hash(const char *in_s) {                                   // 计算哈希值
    int seed = 31, hash = 0;
    for (int i = 0; in_s[i]; i += 1) { hash = hash * seed + in_s[i]; }
    return hash & 0x7fffffff;
}

void regist_command(ShellFunc in_func, const char *in_cmd) {        // 注册函数到哈希表中
    int hash_i = BKDR_hash(in_cmd) % HT_SIZE;
    // printf("%s has hash code %d\n", in_cmd, hash_i);
    FuncList *new_func = (FuncList *) malloc(sizeof(FuncList));
    new_func -> func = in_func;
    new_func -> cmd = strdup(in_cmd);
    if (funcs_htable[hash_i] == NULL) {
        funcs_htable[hash_i] = new_func;
        return ;
    }
    new_func -> node.next = &(funcs_htable[hash_i] -> node);    // 拉链法解决哈希碰撞
    funcs_htable[hash_i] = new_func;
    return ;
}

ShellFunc getFunc(const char *in_cmd) {                             // 从哈希表中查找命令
    int hash_i = BKDR_hash(in_cmd) % HT_SIZE;
    FuncList *s_func = funcs_htable[hash_i];
    // int index = 0;
    while (s_func) {
        if (strcmp(s_func -> cmd, in_cmd) == 0) {
            return s_func -> func;
        }
        // index += 1;
        s_func = (FuncList *) (s_func -> node.next);
    }
    return NULL;
}

void SET_CMD_HEAD(const char * head) { 
    if (strcmp(head, "")) {
        cmd_head = strdup(head);
    }
}

void SET_EXIT_CMD(const char * e_cmd) { 
    if (strcmp(e_cmd, "")) {
        exit_cmd = strdup(e_cmd);
    }
}

void RUN_SHELL() {
    char input_cmd[100] = {0};
    while (1) {
        printf("\e[01;34m%s > \e[0m", cmd_head);
        scanf("%s", input_cmd);

        if (strcmp(input_cmd, exit_cmd) == 0) { 
            printf("Bye!\n");
            break; 
        }
        ShellFunc cmd_func = getFunc(input_cmd);
        if (cmd_func != NULL) { cmd_func(); }
        else { printf("\e[01;31mcommand %s not found!\e[0m", input_cmd); }
        printf("\n");
    }
    return ;
}

