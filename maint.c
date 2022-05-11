#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "utils_v1.h"
#include "const.h"

/**
 * Créer la mémoire partagée et un sémaphore.
 * RES : La mémoire partagée ainsi qu'un sémaphore ont été créés.
 */
void createResources();
/**
 * Détruit la mémoire partagée et le sémaphore.
 * RES : La mémoire partagée ainsi qu'un sémaphore ont été détruits.
 */
void deleteResources();
/**
 * Bloque la mémoire partagée durant une certaine durée.
 *  PRE : @param opt : Durée durant laquelle la mémoire partagée sera bloquée.
 */
void reserveResources(int opt);

int main(int argc, const char** argv) {
	if (argc < 2) {
		printf("Respectez la synthaxe de la fonction : maint type\n");
		exit(0);
	}
	printf("Bienvenu sur le programme de maintenance. \n");
	int type = atoi(argv[1]);
	int opt;
	if(argc==3){
		opt = atoi(argv[2]);
	}else if(argc<3 && type==3){
		perror("Veuillez entrer une durée de maintenance et relancer le programme\n");
		exit(-1);
	}
	switch (type) {
		case 1:
			createResources();
			break;
		case 2:
			deleteResources();
			break;
		case 3:
			reserveResources(opt);
			break;
	}
	exit(0);
}

void createResources(){
	int shmid = sshmget(SHM_KEY, SHM_SIZE*sizeof(int), IPC_CREAT | IPC_EXCL | PERM);
	int semid = sem_create(SEM_KEY, 1, PERM, 1);
	printf("Voici l'identifiant de la mémoire partagée : %d\n", shmid);
	printf("Voici l'identifiant du sémaphore : %d\n", semid );
}

void deleteResources(){
	int shmid = sshmget(SHM_KEY, SHM_SIZE*sizeof(int), 0);
	sshmdelete(shmid);
	printf("La mémoire partagée a été détruite !\n");
	int semid = sem_get(SEM_KEY, 1);
	sem_delete(semid);
	printf("Le sémaphore a été détruit !\n");
}

void reserveResources(int opt){
	int semid = sem_get(SEM_KEY,1);
	sem_down0(semid);
	sleep(opt);
	printf("Fin de la maintenance.\n");
	sem_up0(semid);
	exit(1);
}