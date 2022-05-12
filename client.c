#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "utils_v1.h"
#include "virement.h"
#include "const.h"

volatile sig_atomic_t end = false;

//PRE  : commande (commande envoyée dans le prompt de type "+/* n2 somme")
//       num (numéro de compte de l'envoyeur)
//POST : commande et num pas modifiés
//RES  : Renvoie un virement
Virement initVirement(char* commande, int num);

//PRE  : adr (chaine de caractère contenant l'adresse du serveur)
//       port (entier contenant le port du serveur)
//POST : adr et post pas modifiés
//RES  : Crée la connexion avec le serveur et renvoie le sockfd
int initSocketClient(char * adr, int port);

//PRE  : pipe (le pipe)
//       delay (un entier contenant le temps entre chaque batement)
//POST : pipe et delay pas modifiés
//RES  : Envoie un signal au service de virement récurent
//       pour lui indiquer d'envoyer les viremnets récurents
void minuteur (void *pipe, void *delay, void *set);

//PRE  : pipe (le pipe)
//       adr (chaine de caractère contenant l'adresse du serveur)
//       port (entier contenant le port du serveur)
//POST : aucun paramètre n'est modifiés
//RES  : Envoie un tableau de virements recurents à chaque signal envoyé par le minuteur
void virement_recurent (void *pipe, void *adr, void *port);

int main(int argc, char *argv[]) {
    sigset_t set;
    ssigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    ssigprocmask(SIG_BLOCK, &set, NULL);

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
    int minuteur_pid = fork_and_run3(minuteur, pipefd, &delay, &set);
    fork_and_run3(virement_recurent, pipefd, adr, &port);

    ssigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    ssigprocmask(SIG_UNBLOCK, &set, NULL);

    //Cloture descripteur pour lecture
    sclose(pipefd[0]);

    //Prompt
    printf("Bienvenue dans le service de virement !\n");

    while(!end) {

        char commande[TAILLE_MAX_COMMANDE];
        printf("Veuillez entrer une commande\n");
        sread(0, commande, TAILLE_MAX_COMMANDE);

        if(commande[0] == 'q') {

            //Fermeture des programmes
            end = true;
            skill(minuteur_pid, SIGUSR1);
            struct Virement virementFin = { FIN_PROGRAMME, FIN_PROGRAMME, FIN_PROGRAMME};
            swrite(pipefd[1], &virementFin, sizeof(virementFin));
        }
        else if(commande[0] == '+') {

            //Création du virement
            Virement virement = initVirement(commande, num);

            //Création du socket
            int sockfd = initSocketClient(adr, port);

            //Envoie un 0 pour signaler un virement simple
            int signal = 0;
            nwrite(sockfd, &signal, sizeof(int));

            //Envoyez le virement au serveur
            nwrite(sockfd, &virement, sizeof(virement));

            //Récupérer le solde du compte
            int solde;
            sread(sockfd, &solde, sizeof(solde));

            //Fermeture de la connexion
            sclose(sockfd);

            //Affichage du solde
            printf("Votre solde actuel : %d\n", solde);
        }
        else if(commande[0] == '*') {

            //Création du virement
            Virement virement = initVirement(commande, num);

            //Envoie du virement dans le service de virement récurent
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

Virement initVirement(char* commande, int num) {

    char* tabArguments[3];
    int i = 0;
    char *strTok = strtok(commande, " ");
    while ( strTok != NULL  && i < 3) {
        tabArguments[i] = strTok;
        i++;
        strTok = strtok ( NULL, " ");
    }

    struct Virement virement = { num,atoi(tabArguments[1]),  atoi(tabArguments[2])};
    return virement;
}

int initSocketClient(char * adr, int port) {
    int sockfd = ssocket();

    sconnect(adr, port, sockfd);

    return sockfd;
}

void sigusr1handler() {
    end = true;
}

void minuteur (void *pipe, void *delay, void *set) {

    ssigaction(SIGUSR1, sigusr1handler);

    ssigemptyset(set);
    sigaddset(set, SIGUSR1);
    ssigprocmask(SIG_UNBLOCK, set, NULL);

    int *pipefd = pipe;
    int *delayMinuteur = delay;

    //Fermeture du descripteur en lecture
    sclose(pipefd[0]);

    struct Virement virement = { ENVOIE_OK, ENVOIE_OK, ENVOIE_OK};

    while(!end) {
        sleep(*delayMinuteur);

        //Envoie du battement au service de virement récurent
        swrite(pipefd[1], &virement, sizeof(virement));
    }

    //Fermeture du descripteur en écriture
    sclose(pipefd[1]);
}

void virement_recurent (void *pipe, void *adr, void *port) {

    int *pipefd = pipe;
    int *portServeur = port;

    //Close du descripteur en écriture
    sclose(pipefd[1]);

    Virement virement;
    Virement tabVirement[MAX_NBR_VIREMENT];

    int nbrVirement = 0;

    while(!end) {

        sread(pipefd[0], &virement, sizeof(virement));

        if(virement.compteReceveur == ENVOIE_OK && nbrVirement > 0) {

            int sockfd = initSocketClient(adr, *portServeur);

            //Envoie du nombre de virement au serveur qui signal l'envoie prochain d'une table de virement
            nwrite(sockfd, &nbrVirement, sizeof(int));

            //Envoie d'un tableau de virement
            nwrite(sockfd, &tabVirement, sizeof(tabVirement));

            sclose(sockfd);
        }
        else if(virement.compteReceveur == FIN_PROGRAMME) {
            end = true;
        }
        else if(virement.compteReceveur != ENVOIE_OK && virement.compteReceveur != FIN_PROGRAMME) {

            //Ajout du virement dans la table de virement
            tabVirement[nbrVirement] = virement;
            nbrVirement++;
        }
    }
    printf("Fermeture virement recurent\n");

    //Close du descripteur en lecture
    sclose(pipefd[0]);
}