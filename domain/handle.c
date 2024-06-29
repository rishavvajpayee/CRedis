#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "handle.h"

KeyValue kv_store[100];
int kv_count = 0;

char *handle(char *buffer, int client_socket, int server_fd, fd_set master_set, char *result){
    memset(result, 0, 1028);
    snprintf(result, 1028, "%s\n", buffer);
    if (strncmp(buffer, "nuke", 4) == 0) {
        printf("Received command to nuke. Terminating server...\n");
        close(client_socket);
        FD_CLR(client_socket, &master_set);
        close(server_fd);
        exit(EXIT_SUCCESS);
    }

    if (strncmp(buffer, "SET",3) == 0){
        char key[MAX_KEY_LEN], value[MAX_VALUE_LEN];
        if (sscanf(buffer, "SET %s %s", key, value) == 2){
            if (kv_count < 100){
                strncpy(kv_store[kv_count].key, key, MAX_KEY_LEN - 1);
                strncpy(kv_store[kv_count].value, value, MAX_VALUE_LEN - 1);                        
                kv_count++;

                strcpy(result, "OK\n");
            } else {
                strcpy(result, "store limit reached\n");
            }
        } else {
            strcpy(result, "Invalid SET command\n");
        }
        return result;
    }

    if (strncmp(buffer, "GET", 3) == 0){
        char key[MAX_KEY_LEN];
        if (sscanf(buffer, "GET %s", key) == 1){
            for (int i=0; i < kv_count; i++){
                if (strcmp(kv_store[i].key, key) == 0){
                    snprintf(result, 1028, "%s\n", kv_store[i].value);
                    return result;
                }
            }
            strcpy(result, "key not found\n");
        } else {
            strcpy(result, "Invalid GET command\n");
        }
        return result;
    }

    if (strncmp(buffer, "DEL",3) == 0){
        char key[MAX_KEY_LEN];
        if (sscanf(buffer, "DEL %s", key) == 1){
            for (int i = 0; i < kv_count; i++){
                if (strcmp(kv_store[i].key, key) == 0){
                    for (int j = i; j < kv_count; j++){
                        strcpy(kv_store[j].key, kv_store[j+1].key);
                        strcpy(kv_store[j].value, kv_store[j+1].value);
                    }
                    kv_count--;
                    snprintf(result, 20, "DELETED %s", key);
                    return result;
                }
            }
            strcpy(result, "key not found\n");
        } else {
            strcpy(result, "Invalid DEL command\n");
        }
        return result;
    }
    return result;
}

void ping_pong(char *host, int port){
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host);
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3)<0){
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server running on http://%s:%d", host, port);
    int addrlen = sizeof(address);
    int new_socket;
    char buffer[1024] = {0};
    char response[1024] = {0};
    while (1){
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket <=0){
            printf("Error in accepting connections");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        printf("Connection Accepted \n");
        while (1){
            int valread = read(new_socket, buffer, 1024);
            if (valread <= 0){
                if (valread == 0){
                    printf("Client disconnected !");
                } else {
                    printf("Socket Error");
                }
                break;
            }
            printf("Message recieved : %s\n", buffer);
            if (strncmp(buffer, "nuke", 4) == 0) {
                printf("Received command to nuke. Terminating server...\n");
                close(new_socket);
                break;
            }
            printf("Message recieved : %s\n", buffer);
            char *response = buffer;
            send(new_socket, response, strlen(response), 0);
            printf("response sent!\n");
            memset(buffer, 0, sizeof(buffer));
        }
        close(new_socket);
    }
    close(server_fd);
}