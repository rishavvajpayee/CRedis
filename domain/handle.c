#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void start_server(char *host, int port){
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

    // ACCEPT & HANDLE NEW CONNECTIONS:
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