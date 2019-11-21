#include "common.h"
#include <pthread.h>
static const bool USE_THREADS = true;

struct connection {
    int handle;
    char *type;
};

void * runCommand(struct connection * c) {
    int newConnection = c->handle;
    const char *cmd = c->type;
    char *lineptr = NULL;
    size_t n = 0;
    size_t sz = 0;
    FILE * fp = popen(cmd, "r");
    while ((sz = getline(&lineptr, &n, fp) != EOF)) {
        while (sz > 0) {
            message_t message = {0};
            if (sz < sizeof(message.data)) {
                strcpy(&message.data, lineptr);
                message.size = sz;
                sz = 0;
                send(newConnection, &message, sizeof(message), MSG_MORE);
            } else {
                strcpy(&message.data, lineptr);
                lineptr+=sizeof(message.data);
                sz -= sizeof(message.data);
                message.size = sizeof(message.data);
                send(newConnection, &message, sizeof(message), 0);
            }
        }
    }

    pclose(fp);
    close(newConnection);
        free(lineptr);
    return NULL;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (sockfd == -1) {
        fprintf(stderr, "Unable to allocate socket\n");
        return 1;
    }

    struct sockaddr_in addressPort = {.sin_family =  AF_INET, .sin_port = htons(P1_PORT), .sin_addr.s_addr = INADDR_ANY};

    if (bind(sockfd, (const struct sockaddr *)&addressPort, sizeof(addressPort)) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return 1;
    }

    if (listen(sockfd, 100) == -1) {
        fprintf(stderr, "listen: %s\n", strerror(errno));
        return 1;
    }
    size_t sz = 0;
    while (true) {
        int tmp = sizeof(addressPort);
        int newConnection = accept(sockfd, (struct sockaddr *)&addressPort, &tmp);
        if (newConnection < 0) {
            fprintf(stderr, "accept: %s\n", strerror(errno));
            continue;
        } else {
            fprintf(stderr, "accepting a new connection\n");
        }
        command_t command = { 0 };
        recv(newConnection, &command, sizeof(command), 0);
        switch (command.type) {
            case DATE_CMD:
            {
                static pthread_t thread;
                pthread_create(&thread, NULL, runCommand, &(struct connection){newConnection, "exec date"});
                pthread_detach(thread);
            }
            break;
            case UPTIME_CMD:
            {
                if (USE_THREADS) {
                    static pthread_t thread;
                    pthread_create(&thread, NULL, runCommand, &(struct connection){newConnection, "exec uptime"});
                    pthread_detach(thread);
                } else {
                    runCommand(&(struct connection){newConnection, "exec uptime"});
                }
            }
            break;
            case MEMUSE_CMD:
            {
                if (USE_THREADS) {
                    static pthread_t thread;
                    pthread_create(&thread, NULL, runCommand, &(struct connection){newConnection, "exec free"});
                    pthread_detach(thread);
                } else {
                    runCommand(&(struct connection){newConnection, "exec free"});
                }
            }
            break;
            case NETSTAT_CMD:
            {
                if (USE_THREADS) {
                    static pthread_t thread;
                    pthread_create(&thread, NULL, runCommand, &(struct connection){newConnection, "exec netstat"});
                    pthread_detach(thread);
                } else {
                    runCommand(&(struct connection){newConnection, "exec netstat"});
                }
            }
            break;
            case USERS_CMD:
            {
                if (USE_THREADS) {
                    static pthread_t thread;
                    pthread_create(&thread, NULL, runCommand, &(struct connection){newConnection, "exec who"});
                    pthread_detach(thread);
                } else {
                    runCommand(&(struct connection){newConnection, "exec who"});
                }
            }
            break;
            case RUNNINGPROCS_CMD:
            {
                if (USE_THREADS) {
                    static pthread_t thread;
                    pthread_create(&thread, NULL, runCommand, &(struct connection){newConnection, "exec free"});
                    pthread_detach(thread);
                } else {
                    runCommand(&(struct connection){newConnection, "exec free"});
                }
            }
            break;
            default:
                fprintf(stderr, "Invalid command");;
            break;
        }
    }

    close(sockfd);
}
