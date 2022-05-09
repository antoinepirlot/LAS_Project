#include <stdio.h>
#include <stdlib.h>
#include "utils_v1.h"
#include "const.h"

int pdr(int numCompte, int montant);

int main(int argc, char *argv[])
{
	if(argc != 3) {
		printf("Respectez la synthaxe de la fonction : pdr [compte] [montant]\n");
		exit(0);
	}

	int numCompte = atoi(argv[1]);
	int montant = atoi(argv[2]);

	printf("Num Compte : %d et Montant : %d\n", numCompte, montant);
	int soldeRestant = pdr(numCompte, montant);
	printf("Voici le solde restant sur le compte %d : %d\n", numCompte, soldeRestant);
	return 0;
}

int pdr(int numCompte, int montant){
	printf("debut pdr\n");
	int sem_id = sem_create(SEM_KEY, 1, PERM, 0);
	int shmid = sshmget(SHM_KEY, SHM_SIZE* sizeof(int), 0);
	int* accounts = sshmat(shmid);
	printf("avant semdown\n");
	sem_down0(sem_id);
	printf("après semdown\n");
	accounts[numCompte] += montant;
	int solde = accounts[numCompte];
	printf("solde = %d\n", solde);
	sem_up0(sem_id);
	return solde;
}