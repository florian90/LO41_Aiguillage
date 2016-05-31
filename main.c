#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "structures.h"
#include "aiguillage.h"
#include "train.h"

#define NB_THREAD 5

int initAiguilleur();
void *fonc_train(void* p);
int lancerTrains();
void erreur(const char *str, int lvl);

Aiguillage aiguillage;
pthread_t pid[NB_THREAD];

int main(int argc, char *argv[])
{
    initAiguillage();
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
        usleep(TEMPS);
    }
    for(i=0;i<nbr;i++)
    {
        pthread_join(pid[i], NULL);
    }
    puts("Fin :");
    printAiguillage();
    return 0;
}

void *fonc_train(void* p)
{
    Train train = initTrain((int)p);
    printf("Création de : ");
    printTrain(&train);
    while(!fini(&train))
    {
        // demande acces au suivant
        demandeAcces(&train);
        printf("Train %d entre dans %d\n", train.id, train.position);
        //fflush(stdout);

        usleep(TEMPS);

        // avance le train
        train.position += suivant(&train);
        //printTrain(&train);

        usleep(TEMPS);

        // libere l'acces du précédent
        libererAcces(&train);
        printf("Train %d sort de %d\n", train.id, train.position-suivant(&train));
        //fflush(stdout);
        //printTrain(&train);
    }
    printf("Train %d terminé\n", train.id);
    pthread_exit(0);
}

void erreur(const char *str, int lvl)
{
    perror(str);
    exit(lvl);
}
