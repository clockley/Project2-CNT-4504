#include "common.h"

void runCommand(int newConnection, char **lineptr, size_t *n, const char *cmd) {
    size_t sz = 0;
    FILE * fp = popen(cmd, "r");
    while ((sz = getline(lineptr, n, fp) != EOF)) {
        while (sz > 0) {
            message_t message = {0};
            if (sz < 448) {
                strcpy(&message.data, *lineptr);
                message.size = strlen(&message.data);
                printf("%s xxx %li\n", *lineptr,  message.size);
                message.size = sz;
                sz = 0;
                write(newConnection, &message, 512);
            } else {
                strcpy(&message.data, *lineptr);
                *lineptr+=448;
                sz -= 448;
                message.size = 448;
                send(newConnection, &message, 512, MSG_MORE);
            }
        }
    }
    pclose(fp);
    close(newConnection);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        fprintf(stderr, "Unable to allocate socket\n");
        return 1;
    }

    struct sockaddr_in addressPort = {.sin_family =  AF_INET, .sin_port = htons(P1_PORT), .sin_addr.s_addr = INADDR_ANY};

    if (bind(sockfd, (const struct sockaddr *)&addressPort, sizeof(addressPort)) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return 1;
    }

    if (listen(sockfd, 3) == -1) {
        fprintf(stderr, "Feeling hard headed... ears on vacation\n");
        return 1;
    }
    char * lineptr = NULL;
    size_t n = 0, sz = 0;
    while (true) {
        int tmp = sizeof(addressPort);
        int newConnection = accept(sockfd, (struct sockaddr *)&addressPort, &tmp);
        if (newConnection < 0) {
            fprintf(stderr, "me no accept\n");
            continue;
        }
        command_t command = { 0 };
        recv(newConnection, &command, sizeof(command), 0);
        switch (command.type) {
            case DATE_CMD:
            runCommand(newConnection, &lineptr, &n, "date");
            break;
            case UPTIME_CMD:
            runCommand(newConnection, &lineptr, &n, "uptime");
            break;
            case MEMUSE_CMD:
            runCommand(newConnection, &lineptr, &n, "free");
            break;
            case NETSTAT_CMD:
            runCommand(newConnection, &lineptr, &n, "netstat");
            break;
            case USERS_CMD:
            runCommand(newConnection, &lineptr, &n, "who");
            break;
            case RUNNINGPROCS_CMD:
            runCommand(newConnection, &lineptr, &n, "ps ax");
            break;
            default:
                ;
            break;
        }
    }
    free(lineptr);
    close(sockfd);
}