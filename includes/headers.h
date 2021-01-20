#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENTS 50
#define MAX_USER 1000
#define PORT 8888
#define MY_IP "47.93.197.19"
