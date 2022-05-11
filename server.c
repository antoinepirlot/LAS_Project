#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils_v1.h"
#include "virement.h"
#include "const.h"

#define BACKLOG 5
#define MAX_VIREMENTS 10

volatile sig_atomic_t end = false;

int initSocket(int port);

void endServer();

void handleUniqueVirement(int newSockFd, int semId, int shmId);

void handleMultipleVirements(int newSockFd, int semId, int shmId, int nbVirementsRecurrents);

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

    shmId = sshmget(SHM_KEY, SHM_SIZE * sizeof(int), 0);
    semId = sem_get(SEM_KEY, 1);

    ssigprocmask(SIG_UNBLOCK, &set, NULL);
    while (!end) {
        int newSockFd = saccept(sockFd);
        int nbVirementsRecurrents;
        sread(newSockFd, &nbVirementsRecurrents, sizeof(int));
        //0 virement unique
        if (nbVirementsRecurrents == 0 ) {
            handleUniqueVirement(newSockFd, semId, shmId);
            printf("Virement unique effectué\n");
        } else {        //1 2 ... virement regulier et correspond  à length
            handleMultipleVirements(newSockFd, semId, shmId, nbVirementsRecurrents);
            printf("Virements récurrents effectués\n");
        }
        sclose(newSockFd);
    }
    sclose(sockFd);
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

void handleUniqueVirement(int newSockFd, int semId, int shmId) {
    Virement virement;
    sread(newSockFd, &virement, sizeof(Virement));
    sem_down0(semId);
    int *accounts = sshmat(shmId);
    accounts[virement.compteEnvoyeur] -= virement.somme;
    accounts[virement.compteReceveur] +=virement.somme;
    int solde = accounts[virement.compteEnvoyeur];
    sshmdt(accounts);
    sem_up0(semId);
    nwrite(newSockFd, &solde , sizeof(int));
}

void handleMultipleVirements(int newSockFd, int semId, int shmId, int nbVirementsRecurrents) {
    Virement *tabVirements = malloc(nbVirementsRecurrents * sizeof(Virement));
    checkNull(tabVirements, "Erreur lors du malloc de la table de virements");
    sread(newSockFd, tabVirements, sizeof(tabVirements));
    sem_down0(semId);
    int *accounts = sshmat(shmId);
    for (int i = 0; i < nbVirementsRecurrents; i++) {
        accounts[tabVirements[i].compteEnvoyeur] -= tabVirements[i].somme;
        accounts[tabVirements[i].compteReceveur] += tabVirements[i].somme;
    }
    sshmdt(accounts);
    sem_up0(semId);
}