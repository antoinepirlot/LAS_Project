#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "utils_v1.h"
#include "virement.h"

#define BACKLOG 5
#define MAX_VIREMENTS 10

volatile sig_atomic_t end = false;

int initSocket(int port);

int main(int argc, char **argv) {
    int port = atoi(argv[1]);
    int sockFd = initSocket(port);
    Virement tabVirements[MAX_VIREMENTS], virement;
    int nbVirements = 0;
    printf("Le serveur tourne sur le port: %d\n", port);
    while(!end) {
        int newSockFd = saccept(sockFd);
        sread(newSockFd, &virement, sizeof(Virement));
        printf("Virement pour : %d€", virement.somme);
        nbVirements++;
    }
    sclose(sockFd);
    exit(0);
}

int initSocket(int port) {
    int sockfd = ssocket();
    sbind(port, sockfd);
    slisten(sockfd, BACKLOG);
    return sockfd;
}