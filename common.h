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
#include <signal.h>
#include <sys/time.h>

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

static uint64_t getTimestampInMicroseconds(struct timespec * start, struct timespec * end) {
	struct timespec tmp;
	if (end->tv_nsec-start->tv_nsec < 0) {
		tmp.tv_sec = end->tv_sec-start->tv_sec-1;
		tmp.tv_nsec = 1000000000+end->tv_nsec-start->tv_nsec;
	} else {
		tmp.tv_sec = end->tv_sec-start->tv_sec;
		tmp.tv_nsec = end->tv_nsec-start->tv_nsec;
	}
    struct timeval tv;
    TIMESPEC_TO_TIMEVAL(&tv,  &tmp);
	return 1000000 * tv.tv_sec + tv.tv_usec;
}

#endif