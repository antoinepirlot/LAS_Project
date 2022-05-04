
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "utils_v1.h"

void createResources();
void deleteResources();
void reserveResources(int opt);

int main(int argc, const char** argv) {
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
	return 0;
}

void createResources(){
	int shmid = sshmget(SHM_KEY, SHM_SIZE*sizeof(int), IPC_CREAT | IPC_EXCL | 0644);
	printf("Voici l'adresse de la mémoire partagée : %d\n", shmid);
}

void deleteResources(){
	int shmid = sshmget(SHM_KEY, SHM_SIZE*sizeof(int), 0);
	sshmdelete(shmid);
	printf("La mémoire partagée a été détruite !\n");
}

void reserveResources(int opt){
	printf("%d", opt);
}