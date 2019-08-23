#include "common.h"

void runCommand(int newConnection, char *lineptr, size_t *n, const char *cmd) {
    size_t sz = 0;
    FILE * fp = popen(cmd, "r");
    while ((sz = getline(&lineptr, n, fp) != EOF)) {
        while (sz > 0) {
            message_t message = {0};
            if (sz < 448) {
                message.size = sz;
                memcpy(message.data, lineptr, sz);
                sz = 0;
                send(newConnection, &message, 512, 0);
            } else {
                memcpy(message.data, lineptr, 448);
                lineptr+=448;
                sz -= 448;
                message.size = 448;
                send(newConnection, &message, 512, MSG_MORE);
            }
        }
    }
    pclose(fp);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        fprintf(stderr, "Unable to allocate socket");
        return 1;
    }

    struct sockaddr_in addressPort = {.sin_family =  AF_INET, .sin_port = htons(P1_PORT)};
    inet_pton(AF_INET, SERVER_IP, &addressPort.sin_addr);

    if (bind(sockfd, (const struct sockaddr *)&addressPort, sizeof(addressPort)) == -1) {
        fprintf(stderr, "Can't bind....can't continue...");
        return 1;
    }

    if (listen(sockfd, 64) == -1) {
        fprintf(stderr, "Feeling hard headed... ears on vacation");
        return 1;
    }
    char * lineptr = NULL;
    size_t n = 0, sz = 0;
    while (true) {
        int tmp = sizeof(addressPort);
        int newConnection = accept(sockfd, (struct sockaddr *)&addressPort, &tmp);
        if (newConnection < 0) {
            fprintf(stderr, "me no accept");
            continue;
        }
        command_t command = { 0 };
        recv(newConnection, &command, sizeof(command), 0);
        switch (command.type) {
            case DATE_CMD:
            runCommand(newConnection, lineptr, &n, "date");
            break;
            case UPTIME_CMD:
            runCommand(newConnection, lineptr, &n, "uptime");
            break;
            case MEMUSE_CMD:
            runCommand(newConnection, lineptr, &n, "free");
            break;
            case NETSTAT_CMD:
            runCommand(newConnection, lineptr, &n, "netstat");
            break;
            case USERS_CMD:
            runCommand(newConnection, lineptr, &n, "who");
            break;
            case RUNNINGPROCS_CMD:
            runCommand(newConnection, lineptr, &n, "ps ax");
            break;
            default:
                ;
            break;
        }
        close(newConnection);
    }
    free(lineptr);
    close(sockfd);
}