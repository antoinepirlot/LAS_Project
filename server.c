#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "utils_v1.h"
#include "virement.h"
#include "const.h"

#define BACKLOG 5
#define MAX_VIREMENTS 10

volatile sig_atomic_t end = false;

int initSocket(int port);

int main(int argc, char **argv) {
    int nbVirements = 0, nbSockFd = 0;
    int port = atoi(argv[1]);
    int sockFd = initSocket(port);
    Virement tabVirements[MAX_VIREMENTS], virement;
    struct pollfd fds[1024];
    bool fds_invalid[1024];
    printf("Le serveur tourne sur le port: %d\n", port);

    fds[nbSockFd].fd = sockFd;
    fds[nbSockFd].events = POLLIN;
    fds_invalid[nbSockFd] = false;
    nbSockFd++;

    while (!end) {
        spoll(fds, nbSockFd, 0);
        if (fds[0].revents & POLLIN & !fds_invalid[0]) {
            int newSockFd = saccept(sockFd);
            fds[nbSockFd].fd = newSockFd;
            fds[nbSockFd].events = POLLIN;
            nbSockFd++;
            fds_invalid[nbSockFd] = false;
        }
        for (int i = 1; i < nbSockFd; i++) {
            if (fds[i].revents & POLLIN && !fds_invalid[i]) {
                sread(fds[i].fd, &virement, sizeof(virement));
                nwrite(fds[i].fd, &virement, sizeof(virement));
                printf("Virement pour : %d€\n", virement.somme);
                nbVirements++;
            }
            //sleep(2);
            sclose(fds[i].fd);
            fds_invalid[i] = true;
        }
    }
    exit(0);
}

int initSocket(int port) {
    int sockfd = ssocket();
    sbind(port, sockfd);
    slisten(sockfd, BACKLOG);
    return sockfd;
}