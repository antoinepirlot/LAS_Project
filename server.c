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

void endServer();

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Server prend un argument qui correspond au port.\n"
               "Par exemple: ./server 8080\n");
    }
    sigset_t set;
    ssigemptyset(&set);
    ssigaddset(&set, SIGINT);
    ssigprocmask(SIG_BLOCK, &set, NULL);
    ssigaction(SIGINT, endServer);

    int nbVirements = 0, nbSockFd = 0, shmId;
    int port = atoi(argv[1]);
    int sockFd = initSocket(port);

    Virement tabVirements[MAX_VIREMENTS], virement;

    struct pollfd fds[1024];

    bool fds_invalid[1024];

    shmId = sshmget(SHM_KEY, SHM_SIZE, 0);
    printf("Le serveur tourne sur le port: %d\n", port);

    fds[nbSockFd].fd = sockFd;
    fds[nbSockFd].events = POLLIN;
    fds_invalid[nbSockFd] = false;
    nbSockFd++;

    ssigprocmask(SIG_UNBLOCK, &set, NULL);
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
                void *addr = sshmat(shmId);
                sread(fds[i].fd, &virement, sizeof(virement));
                nwrite(fds[i].fd, &virement, sizeof(virement));
                printf("Virement pour : %d€\n", virement.somme);
                nbVirements++;
                sshmdt(addr);
            }
            sclose(fds[i].fd);
            fds_invalid[i] = true;
        }
    }
    printf("Fin du serveur.\n");
    exit(0);
}

int initSocket(int port) {
    int sockfd = ssocket();
    sbind(port, sockfd);
    slisten(sockfd, BACKLOG);
    return sockfd;
}

void endServer() {
    end = true;
}