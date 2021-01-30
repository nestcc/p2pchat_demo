#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>

#define MAX_EVENTS 50
#define MAX_USER 1000
#define PORT 8888
#define MY_IP "47.93.197.19"
