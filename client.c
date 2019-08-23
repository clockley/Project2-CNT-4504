#include "common.h"

long long * promptForNumber(char * msg) {
    printf("%s", msg);
    char *lineptr = NULL;
    char *endptr = NULL;
    size_t n = 0;
    long long * ret = calloc(1, sizeof(long long));;
    while (getline(&lineptr, &n, stdin) != EOF) {
        errno = 0;
        if ((*ret = strtoll(lineptr, &endptr, 10))) {
            if (*endptr == *lineptr) {
                printf("Invalid input please try again: ");
                continue;
            } else if ((errno == ERANGE && (*ret == LLONG_MAX || *ret == LLONG_MIN)) || (errno != 0 && *ret == 0)) {
                printf("Invalid input please try again: ");
                continue;
            } else {
                free(lineptr);
                return ret;
            }
        } else {
            printf("Invalid input please try again: ");
        }
    }
    free(lineptr);
    return NULL;
}

void printMenu(void) {
    char * options[] = {"Host current Date and Time", "Host uptime", "Host memory use", "Host Netstat", "Host current users", "Host running processes", "Quit", NULL};
    for (int i = 0; options[i] != NULL; ++i) {
        printf("%i.\t%s\n", i+1, options[i]);
    }
}

bool sendCommandAndPrintOutput(char command) {
    command_t cmd = {.type = command};
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        fprintf(stderr, "Unable to allocate socket\n");
        return false;
    }

    struct sockaddr_in addressPort = {.sin_family =  AF_INET, .sin_port = htons(P1_PORT)};
    inet_pton(AF_INET, SERVER_IP, &addressPort.sin_addr);

    int connection = 0;

    if (connect(sockfd, (const struct sockaddr *)&addressPort, sizeof(addressPort)) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return false;
    }

    message_t message = {0};

    if (send(sockfd, &cmd, sizeof(cmd), 0) < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return 1;
    }
    while (read(sockfd, &message, sizeof(message_t))  > 0) {
        printf(&message.data);
    }
    return true;
}

int main(void) {
    printMenu();
    long long * tmp = NULL;
    do {
        long long input = 0;
        if ((tmp = promptForNumber("Please select option: "))) {
            input = *tmp;
        } else {
            free(tmp);
        }
        if (input == 7) {
            return 0;
        }
        sendCommandAndPrintOutput(input);
    } while (true);
}
