#include <stdio.h>
#include <stdlib.h>

#include "utils_v1.h"

int main(int argc, char *argv[])
{	
	if(argc != 3) {
		printf("Respectez la synthaxe de la fonction : pdr [compte] [montant]\n");
		exit(0);
	}
	
	int numCompte = atoi(argv[1]);
	int montant = atoi(argv[2]);

	printf("Num Compte : %d et Montant : %d", numCompte, montant);
	return 0;
}