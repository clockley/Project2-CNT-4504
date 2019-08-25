#ifndef COMMON_H
#define COMMON_H
#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <string.h>

#define CLIENT_IP "127.0.0.1"
#define SERVER_IP "127.0.0.1"

#define DATE_CMD         1
#define UPTIME_CMD       2
#define MEMUSE_CMD       3
#define NETSTAT_CMD      4
#define USERS_CMD        5
#define RUNNINGPROCS_CMD 6
#define CMDREPLY         0
#define P1_PORT 8080

typedef struct  {
    uint64_t size;
    char data[1280-sizeof(uint64_t)];
} message_t;

struct command_t {
    unsigned char type;
    char ret;
};

typedef struct command_t command_t;
#endif