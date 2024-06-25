#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "domain/multiplexing.h"

int main(int argc, char *argv[]){
    char *port_str = getenv("PORT");
    int port;
    char *host = getenv("HOST");

    if (port_str == NULL || host == NULL){
        fprintf(stderr, "Env Variables HOST & PORT are not set through ENV");
    }
    if (argc > 1){
        for (int i=1; i<argc; i++){
            if (!strcmp(argv[i],"-p") && (i + 1) < argc){
                port_str = argv[i+1];
            }
            if (!strcmp(argv[i],"-h") && (i + 1) < argc){
                host = argv[i+1];
            }
        }
    }
    port = atoi(port_str);
    printf("%s : host | %d : port\n", host, port);
    multiplexing(host, port);
    return 0;
  }