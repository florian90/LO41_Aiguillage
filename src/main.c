#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "structures.h"
#include "train.h"
#include "aiguillage.h"

void lancerAiguillage();
void *fonc_train(void* p);
int lancerTrains();
void erreur(const char *str, int lvl);

pthread_t pidAiguilleur;

pthread_t pidTrains[NB_THREAD];
Train_t trains[NB_THREAD];

int main(int argc, char *argv[])
{
    lancerAiguillage();
    lancerTrains(NB_THREAD);
    return 0;
}

void lancerAiguillage()
{
    pthread_create(&pidAiguilleur, 0, initAiguillage, (void *) NULL);
}


int lancerTrains(int nbr)
{
    int i;
    srand(time(0));
    for(i=0;i<nbr;i++)
    {
        pthread_create(&pidTrains[i], 0, (void *(*)(void *))fonc_train, (void*) i);
        //usleep(TEMPS);
    }
    for(i=0;i<nbr;i++)
    {
        pthread_join(pidTrains[i], NULL);
    }
    puts("Fin :");
    printAiguillage();
    return 0;
}

void *fonc_train(void* p)
{
    Train_t train = initRandTrain((int)p);
    //printf("Création de : ");
    printTrain(&train);
    while(!arrive(&train))
    {
        if(avance(&train) != 0)
        {
            //bloquerTrain(&train);
            printf("BLOQUE\n");
            pthread_exit(1);
        }
    }
    printf("Train_t %d terminé\n", (int) train.id);
    pthread_exit(0);
}

/*
 * Fonc aiguilleur :
    While(1)
    {
        parcourir la liste des trains,
        Dès qu'il peut libérer un TGV -> OK

    }
 */

void erreur(const char *str, int lvl)
{
    perror(str);
    exit(lvl);
}
