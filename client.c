#include "common.h"
#include "pool.c"

int com[2] = {0};
long long * numberOfconcurrentRequest;

long long * promptForNumber(char * msg) {
    printf("%s", msg);
    char *lineptr = NULL;
    char *endptr = NULL;
    size_t n = 0;
    static long long ret = 0;
    while (getline(&lineptr, &n, stdin) != EOF) {
        fflush(stdout);
        errno = 0;
        if ((ret = strtoll(lineptr, &endptr, 10))) {
            if (*endptr == *lineptr) {
                printf("Invalid input please try again: ");
                printf("%s", msg);
                continue;
            } else if ((errno == ERANGE && (ret == LLONG_MAX || ret == LLONG_MIN)) || (errno != 0 && ret == 0)) {
                printf("Invalid input please try again: ");
                printf("%s", msg);
                continue;
            } else {
                free(lineptr);
                return &ret;
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

void * sendCommandAndPrintOutput(void * arg) {

    command_t cmd = {.type = (char)arg};
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        return strerror(errno);
    }

    struct sockaddr_in addressPort = {.sin_family =  AF_INET, .sin_port = htons(P1_PORT)};
    inet_pton(AF_INET, SERVER_IP, &addressPort.sin_addr);

    int connection = 0;
    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC_COARSE, &t1);

    if (connect(sockfd, (const struct sockaddr *)&addressPort, sizeof(addressPort)) == -1) {
        return strerror(errno);
    }

    message_t message = {0};

    if (send(sockfd, &cmd, sizeof(cmd), 0) < 0) {
        return strerror(errno);
    }
    char *ptr = NULL;
    size_t sizeloc = 0;
    FILE * output = open_memstream(&ptr, &sizeloc);
    while (read(sockfd, &message, sizeof(message_t))  > 0) {
        fputs_unlocked(&message.data, output);
    }
    clock_gettime(CLOCK_MONOTONIC_COARSE, &t2);
    fclose(output);
    flockfile(stdout);
    fputs_unlocked(ptr, stdout);
    fflush_unlocked(stdout);
    FILE * bench = fopen("bench.txt", "a+");
    fprintf(bench, "Time:\t%lu;Command:\t%c;ClientCount:\t%lli\n", getTimestampInMicroseconds(&t1, &t2), cmd.type, *numberOfconcurrentRequest);
    fclose(bench);
    funlockfile(stdout);
    free(ptr);
    char tmp = 0;
    write(com[1], &tmp, sizeof(tmp));
    return NULL;
}

int main(void) {
    numberOfconcurrentRequest = NULL;
    do {
        numberOfconcurrentRequest = promptForNumber("Enter number of concurrent request: ");
        if (numberOfconcurrentRequest != NULL && *numberOfconcurrentRequest <= 0) {
            printf("Invalid input please try again\n");
            numberOfconcurrentRequest = NULL;
        }
    } while (numberOfconcurrentRequest == NULL);
    {
        __auto_type tmp = *numberOfconcurrentRequest;
        numberOfconcurrentRequest = malloc(sizeof(*numberOfconcurrentRequest));
        *numberOfconcurrentRequest = tmp;
    }

    ThreadPoolNew();

    long long * tmp = NULL;
    do {
        fflush(stdout);
        printMenu();
        long long input = 0;
        if ((tmp = promptForNumber("Please select option: "))) {
            input = *tmp;
        } else {
            break;
        }
        if (input == 7) {
            return 0;
        }

        pipe(com);

        for (__auto_type  i = *numberOfconcurrentRequest; i > 0; --i) {
            ThreadPoolAddTask(sendCommandAndPrintOutput, (void*)input, true);
        }

        for (__auto_type  i = *numberOfconcurrentRequest; i > 0; --i) {
            char tmp = 0;
            read(com[0], &tmp, sizeof(tmp));
        }

    } while (true);

    ThreadPoolCancel();
}
