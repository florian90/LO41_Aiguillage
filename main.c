#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "structures.h"
#include "aiguillage.h"
#include "train.h"

#define NB_THREAD 2

int initAiguilleur();
void *fonc_train(void* p);
int lancerTrains();
void erreur(const char *str, int lvl);

Aiguillage aiguillage;
pthread_t pid[NB_THREAD];

int main(int argc, char *argv[])
{
    printf("Initialisation...\n");
    //printf("Geneation de la voie ferroviaire\n");

    initAiguillage();
    printf("Aiguillage pret...\n");
    lancerTrains(NB_THREAD);

    return 0;
}

int lancerTrains(int nbr)
{
    int i;
    srand(time(0));
    for(i=0;i<nbr;i++)
    {
        pthread_create(&pid[i], 0, (void *(*))fonc_train, (void*) i);
    }
    for(i=0;i<nbr;i++)
    {
        pthread_join(pid[i], NULL);
    }
    return 0;
}

void *fonc_train(void* p)
{
    Train train = initTrain((int)p);
    printTrain(&train);
    while(!fini(&train))
    {
        // demande acces au suivant
        demandeAcces(&train);

        usleep(TEMPS);

        // avance le train
        train.position += suivant(&train);
        //printTrain(&train);

        usleep(TEMPS);

        // libere l'acces du précédent
        libererAcces(&train);
        printTrain(&train);
    }
    printf("Train %d terminé\n", train.id);
    pthread_exit(0);
}

void erreur(const char *str, int lvl)
{
    perror(str);
    exit(lvl);
}
