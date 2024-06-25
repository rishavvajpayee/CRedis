#ifndef HANDLE_H
#define HANDLE_H

#include <sys/select.h>

#define MAX_KEY_LEN 256
#define MAX_VALUE_LEN 256

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} KeyValue;

void ping_pong(char *host, int port);
char *handle(char *buffer, int client_socket, int server_fd, fd_set master_set, char *result);

#endif