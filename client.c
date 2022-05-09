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

//Fils minuteur
void minuteur (void *pipe, void *delay) {

    int *pipefd = pipe;
    //Close du descripteur en écriture
    sclose(pipefd[1]);




    //Close du descripteur en lecture
    sclose(pipefd[0]);
}

//Fils virement recurent
void virement_recurent (void *pipe, void *adr, void *port, void *num) {

    int *pipefd = pipe;
    //Close du descripteur en écriture
    sclose(pipefd[1]);



    //Close du descripteur en lecture
    sclose(pipefd[0]);
}

//Crée le socket
int initSocketClient(char * adr, int port) {
    int sockfd = ssocket();

    sconnect(adr, port, sockfd);

    return sockfd;
}

int main(int argc, char *argv[])
{
    if(argc != 5) {
        printf("Respectez la synthaxe de la fonction : client [adr] [port] [num] [delay]\n");
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
    //Création du socket
    int sockfd = initSocketClient(adr, port);

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

            //Récupérer les arguments de la commande (FONCTION ?)
            char** tabArguments = (char**)malloc(3*sizeof(char*));
            int i = 0;
            char* strTok = strtok(commande, " ");
            while ( strTok != NULL  && i < 3) {
                tabArguments[i] = (char*)malloc((strlen(strTok)+1)*sizeof(char));
                tabArguments[i] = strTok;
                i++;
                strTok = strtok ( NULL, " ");
            }
            struct Virement virement = {num, *tabArguments[1], *tabArguments[2]};

            //Envoyez le virement au serveur
            swrite(sockfd, &virement, sizeof(virement));

            //Récupérer le solde du compte
            int solde;
            sread(sockfd, &solde, sizeof(solde));
            printf("Votre solde actuel : %d\n", solde);
        }
        else if(commande[0] == '*') {

            //Récupérer les arguments de la commande (FONCTION ?)
            char** tabArguments = (char**)malloc(3*sizeof(char*));
            int i = 0;
            char* strTok = strtok(commande, " ");
            while ( strTok != NULL  && i < 3) {
                tabArguments[i] = (char*)malloc((strlen(strTok)+1)*sizeof(char));
                tabArguments[i] = strTok;
                i++;
                strTok = strtok ( NULL, " ");
            }
            struct Virement virement = {num, *tabArguments[1], *tabArguments[2]};
        }
        else {
            printf("Erreur dans la commande\n");
        }
    }

    //Close du descripteur en lecture
    sclose(pipefd[1]);
    //Fermeture de la connexion
    sclose(sockfd);

    return 0;
}

