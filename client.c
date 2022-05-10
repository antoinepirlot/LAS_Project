#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils_v1.h"
#include "virement.h"

#define TAILLE_MAX_COMMANDE 15
#define ENVOIE_OK -1
#define MAX_NBR_VIREMENT 100

//Récupérer les arguments de la commande et crée le virement
Virement initVirement(char* commande, int num) {

    char** tabArguments = (char**)malloc(3*sizeof(char*));
    int i = 0;
    char* strTok = strtok(commande, " ");
    while ( strTok != NULL  && i < 3) {
        tabArguments[i] = (char*)malloc((strlen(strTok)+1)*sizeof(char));
        tabArguments[i] = strTok;
        i++;
        strTok = strtok ( NULL, " ");
    }

    struct Virement virement = { num,atoi(tabArguments[1]),  atoi(tabArguments[2])};
    return virement;
}

//Crée le socket
int initSocketClient(char * adr, int port) {
    int sockfd = ssocket();

    sconnect(adr, port, sockfd);

    return sockfd;
}


//Fils minuteur
void minuteur (void *pipe, void *delay) {

    int *pipefd = pipe;
    int *delayMinuteur = delay;

    //Close du descripteur en lecture
    sclose(pipefd[0]);

    struct Virement virement = { ENVOIE_OK, ENVOIE_OK, ENVOIE_OK};

    while(1) {
        sleep(*delayMinuteur);
        swrite(pipefd[1], &virement, sizeof(virement));
    }

    //Close du descripteur en lecture
    sclose(pipefd[1]);
}

//Fils virement recurent
void virement_recurent (void *pipe, void *adr, void *port, void *num) {

    int *pipefd = pipe;
    int *portServeur = port;

    //Close du descripteur en écriture
    sclose(pipefd[1]);

    Virement virement;
    Virement tabVirement[MAX_NBR_VIREMENT];

    int nbrVirement = 0;

    while(1) {
        sread(pipefd[0], &virement, sizeof(virement));
        printf("Batement Récurent (Virement.compteReceveur = %d)\n", virement.compteReceveur);
        if(virement.compteReceveur == ENVOIE_OK && nbrVirement > 0) {

            //ENVOYER -1

            int sockfd = initSocketClient(adr, *portServeur);
            swrite(sockfd, &tabVirement, sizeof(tabVirement));
            sclose(sockfd);
        }
        else {
            tabVirement[nbrVirement] = virement;
            nbrVirement++;
        }
    }

    //Close du descripteur en lecture
    sclose(pipefd[0]);
}

int main(int argc, char *argv[])
{
    if(argc != 5) {
        printf("Respectez la synthaxe de la fonction : client adr port num delay\n");
        exit(0);
    }

    //Récupération des paramètres
    char * adr = argv[1];
    int port = atoi(argv[2]);
    int num = atoi(argv[3]);
    int delay = atoi(argv[4]);

    //Création du pipe
    int pipefd[2];
    spipe(pipefd);

    //Création des fils
    int minuteur_pid = fork_and_run2(minuteur, pipefd, &delay);
    int virement_recurent_pid = fork_and_run4(virement_recurent, pipefd, adr, &port, &num);

    //Cloture descripteur pour lecture
    sclose(pipefd[0]);

    //Prompt
    printf("Bienvenue dans le service de virement !\n");
    bool fin = false;

    while(!fin) {

        char commande[TAILLE_MAX_COMMANDE];
        printf("Veuillez entrer une commande\n");
        sread(0, commande, TAILLE_MAX_COMMANDE);

        if(commande[0] == 'q') {
            fin = true;
        }
        else if(commande[0] == '+') {
            Virement virement = initVirement(commande, num);
            printf("Contenu virement: Somme=%d, Envoyeur=%d, Receveur=%d\n", virement.somme, virement.compteEnvoyeur, virement.compteReceveur);

            //Création du socket
            int sockfd = initSocketClient(adr, port);

            //Envoyez le virement au serveur
            nwrite(sockfd, &virement, sizeof(virement));

            //Récupérer le solde du compte
            int solde;
            printf("Attente reponse serveur\n");
            sread(sockfd, &solde, sizeof(solde));

            //Fermeture de la connexion
            sclose(sockfd);

            printf("Votre solde actuel : %d\n", solde);
        }
        else if(commande[0] == '*') {

            Virement virement = initVirement(commande, num);
            printf("Contenu virement: %d, %d, %d\n", virement.somme, virement.compteEnvoyeur, virement.compteReceveur);

            swrite(pipefd[1], &virement, sizeof(virement));

            printf("Envoyé dans les virements récurents\n");
        }
        else {
            printf("Erreur dans la commande\n");
        }

    }

    //Close du descripteur en lecture
    sclose(pipefd[1]);

    return 0;
}

