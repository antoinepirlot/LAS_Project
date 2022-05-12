#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils_v1.h"
#include "virement.h"
#include "const.h"

volatile sig_atomic_t end = false;

int initSocket(int port);

/**
 * POST: Mets la variable volatile end à true afin de terminer le serveur
 */
void endServer();

/**
 * PRE: @param newSockFd correspond au file descriptor de la connection acceptée est supérieur à 0
 *      @param semId correspond à l'id du sémaphore, est supérieur à 0
 *      @param shmId correspond à l'id de la mémoire partagée est supérieur à 0
 * POST: Récupère le virement reçu par le client et ajoute la somme au compte receveur
 *       et diminue la somme au compte envoyeur
 */
void handleUniqueVirement(int newSockFd, int semId, int shmId);

/**
 * PRE: @param newSockFd correspond au file descriptor de la connexion avec le client
 *      @param semId correspond à l'id du sémaphore, est supérieur à 0
 *      @param shmId correspond à l'id de la mémoire partagée est supérieur à 0
 *      @param nbVirementsRecurrents correspond au nombre de virements récurrents à traiter
 *  POST: Récupère un tableau de virements et ajoute pour chaque virement la somme au compte receveur
 *        et enleve la somme au compte envoyeur
 */
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

    int port = atoi(argv[1]);
    int sockFd = initSocket(port);
    int shmId = sshmget(SHM_KEY, SHM_SIZE * sizeof(int), 0);
    int semId = sem_get(SEM_KEY, 1);

    while (!end) {
        ssigprocmask(SIG_BLOCK, &set, NULL);
        int newSockFd = saccept(sockFd);
        int nbVirementsRecurrents;
        sread(newSockFd, &nbVirementsRecurrents, sizeof(int));
        if (nbVirementsRecurrents == 0 ) {
            //0 virement unique
            handleUniqueVirement(newSockFd, semId, shmId);
            printf("Virement unique effectué\n");
        } else {
            //1 2 ... virements récurrents et correspond  à length
            handleMultipleVirements(newSockFd, semId, shmId, nbVirementsRecurrents);
            printf("Virements récurrents effectués\n");
        }
        sclose(newSockFd);
        ssigprocmask(SIG_UNBLOCK, &set, NULL);
    }
    sclose(sockFd);
    printf("Fin du serveur.\n");
    exit(0);
    //FIN DU SERVEUR
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
    Virement tabVirements[MAX_NBR_VIREMENT];
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