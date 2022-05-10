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
        printf("Respectez la synthaxe de la fonction : server port\n");

        exit(1);
    }
    sigset_t set;
    ssigemptyset(&set);
    ssigaddset(&set, SIGINT);
    ssigprocmask(SIG_BLOCK, &set, NULL);
    ssigaction(SIGINT, endServer);

    int semId, shmId;
    int port = atoi(argv[1]);
    int sockFd = initSocket(port);

    Virement virement;

    shmId = sshmget(SHM_KEY, SHM_SIZE * sizeof(int), 0);
    semId = sem_get(SEM_KEY, 1);

    ssigprocmask(SIG_UNBLOCK, &set, NULL);
    while (!end) {
        printf("Hey\n");
        int newSockFd = saccept(sockFd);
        printf("Hey\n");
        sread(newSockFd, &virement, sizeof(virement));
        printf("Hey\n");
        sem_down0(semId);
        int *accounts = sshmat(shmId);
        accounts[virement.compteEnvoyeur] = accounts[virement.compteEnvoyeur] - virement.somme;
        accounts[virement.compteReceveur] = accounts[virement.compteReceveur] + virement.somme;
        int solde = accounts[virement.compteEnvoyeur];
        sshmdt(accounts);
        sem_up0(semId); //TODO PROBLEME SOLDE
        nwrite(newSockFd, &solde , sizeof(int));
        printf("Virement pour : %d€\n", virement.somme);
    }
    printf("Fin du serveur.\n");
    exit(0);
}

int initSocket(int port) {
    int sockfd = ssocket();
    sbind(port, sockfd);
    slisten(sockfd, BACKLOG);
    printf("Le serveur tourne sur le port: %d\n", port);
    return sockfd;
}

void endServer() {
    end = true;
}