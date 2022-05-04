#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "utils_v1.h"
#include "virement.h"

#define BACKLOG 5

int initSocket(int port);

int main(int argc, char **argv) {
    //char port = atoi(argv[])
    //printf("Le serveur tourne sur le port: %d\n", *argv[1]);
    int sockfd = initSocket(*argv[1]);
    sclose(sockfd);
    exit(0);
}

int initSocket(int port) {
    int sockfd = ssocket();
    sbind(port, sockfd);
    slisten(sockfd, BACKLOG);
    return sockfd;
}
