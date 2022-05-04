#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils_v1.h"
#include "virement.h"

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

    char * adr = argv[1];
    int port = atoi(argv[2]);
    int num = atoi(argv[3]);
    int delay = atoi(argv[4]);
    printf("adr : %s, port : %d, num : %d, delay : %d\n", adr, port, num, delay);

    //Création du pipe
    int pipefd[2];
    spipe(pipefd);

    //Création des fils
    //TODO

    int sockfd = initSocketClient(adr, port);

    while(true) {
        sleep(10);
    }

    sclose(sockfd);

    return 0;
}

