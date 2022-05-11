#include <stdio.h>
#include <stdlib.h>
#include "utils_v1.h"
#include "const.h"

/**
 * Dépôt ou retrait effectué sur un compte.
 * PRE: @param numCompte : un entier positif désignant le numéro de compte où l'opération est faite.
 * 												Le numéro de compte représente la position du compte dans la mémoire partagée.
 * 		@param montant : un entier positif ou négatif désignant la somme à déposer si l'entier est positif
 * 											ou à retirer si l'entier est négatif.
 *
 * POST: le compte a été crédité ou débité du montant passé en paramètre.
 *
 * RES: @return le nouveau solde du compte passé en paramètre.
 */
int pdr(int numCompte, int montant);

int main(int argc, char *argv[]) {
	if (argc != 3) {
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

int pdr(int numCompte, int montant) {
	int sem_id = sem_get(SEM_KEY, 1);
	int shmid = sshmget(SHM_KEY, SHM_SIZE * sizeof(int), 0);
	int *accounts = sshmat(shmid);
	sem_down0(sem_id);
	accounts[numCompte] += montant;
	int solde = accounts[numCompte];
	sem_up0(sem_id);
	sshmdt(accounts);
	return solde;
}