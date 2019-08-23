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
                return ret;
            }
        } else {
            printf("Invalid input please try again: ");
        }
    }
    return NULL;
}

void printMenu(void) {
    char * options[] = {"Host current Date and Time", "Host uptime", "Host memory use", "Host Netstat", "Host current users", "Host running processes", "Quit", NULL};
    for (int i = 0; options[i] != NULL; ++i) {
        printf("%i.\t%s\n", i+1, options[i]);
    }
}

char * sendCommand(char command) {
    command_t cmd = {.type = command};
    return NULL;
}

int main(void) {
    printMenu();
    long long * input = promptForNumber("Please select option: ");
    free(input);
}
