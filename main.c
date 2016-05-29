#include <stdio.h>
#include <stdlib.h>

#include "structures.h"
#include "aiguillage.h"
#include "train.h"

int initAiguilleur();
int lancerTrains();
void erreur(const char *str, int lvl);

Aiguillage aiguillage;

int main(int argc, char *argv[])
{
    printf("Initialisation...\n");
    //printf("Geneation de la voie ferroviaire\n");

    initAiguillage();
    printf("Aiguillage pret...\n");
    lancerTrains(10);

    return 0;
}

int lancerTrains(int nbr)
{
    // Crée 10 trains puis attends la fin
    // -> Crée 10 threads qui appellent la fonction func_train
    int i
    for(i=0;i<nbr;i++)
        pthread_create(NULL, 0, (void *(*))fonc_train, NULL);
    return 0;
}

void *fonc_train(void* p)
{
    Train train = initTrain();
    while(!fini(train))
    {
        // demande acces au suivant
        demandeAcces(train);

        usleep(TEMPS);

        // avance le train
        train->position += suivant(train);

        // libere l'acces du précédent
        libererAcces(train);
    }
}

void erreur(const char *str, int lvl)
{
    perror(str);
    exit(lvl);
}
