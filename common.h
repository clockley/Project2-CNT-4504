#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DATE_CMD         1
#define UPTIME_CMD       2
#define NETSTAT_CMD      3
#define USERS_CMD        4
#define RUNNINGPROCS_CMD 5

struct command_t {
    unsigned char type;
    char ret;
    size_t outLen;
    char out[];
};

typedef struct command_t command_t;
#endif