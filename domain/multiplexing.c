#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handle.h"

void multiplexing(char *host, int port){
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

    if (listen(server_fd, 3) < 0){
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server running on http://%s:%d\n", host, port);

    fd_set master_set, read_fds;
    int fd_max = server_fd;
    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);

    while (1){
        read_fds = master_set;

        if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1){
            perror("select failed");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= fd_max; i++){
            if (FD_ISSET(i, &read_fds)){
                if (i == server_fd){
                    int new_socket;
                    int addrlen = sizeof(address);
                    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    if (new_socket == -1){
                        perror("accept failed");
                    } else {
                        FD_SET(new_socket, &master_set);
                        if (new_socket > fd_max){
                            fd_max = new_socket;
                        }
                        printf("New connection accepted\n");
                    }
                } else {
                    // Handle data from a client
                    char *response;
                    char buffer[1024] = {0};
                    int valread = read(i, buffer, 1024);
                    if (valread <= 0){
                        if (valread == 0){
                            printf("Client disconnected\n");
                        } else {
                            perror("read error");
                        }
                        close(i);
                        FD_CLR(i, &master_set);
                    } else {
                        printf("Message received: %s\n", buffer);

                        // handle the respose separately
                        char *handled_resp = handle(buffer, i, server_fd, master_set, response);
                        send(i, handled_resp, strlen(handled_resp), 0);
                        printf("Response sent\n");
                    }
                }
            }
        }
    }

    close(server_fd);
}