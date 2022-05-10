#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>

#include "utils_v1.h"
#include "const.h"

void createResources();
void deleteResources();
void reserveResources(int opt);

int main(int argc, const char** argv) {
	if (argc < 2 || argc > 3) {
		printf("Respectez la synthaxe de la fonction : maint type [opt]\n");
		exit(0);
	}
	printf("Hello world! Ici le programme de maintenance. \n");
	int type = atoi(argv[1]);
	int opt;
	if(argc==3){
		opt = atoi(argv[2]);
	}
	printf("test break\n");
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
	printf("Je me réveil\n");
	sem_up0(semid);
	exit(1);
}