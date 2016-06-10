#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "structures.h"
#include "train.h"
#include "aiguillage.h"

void lancerAiguillage();
void *fonc_train(int *p);
int lancerTrains();
void erreur(const char *str, int lvl);

pthread_t pidAiguilleur;

pthread_t pidTrains[NB_THREAD];
Train_t trains[NB_THREAD];

int main(int argc, char *argv[])
{
    if(argc >=2)
        srand(atoi(argv[1]));
    else
    srand(time(NULL));
    lancerAiguillage();
    usleep(100);
    lancerTrains(NB_THREAD);
    return 0;
}

void lancerAiguillage()
{
    pthread_create(&pidAiguilleur, 0, (void *(*)(void *))initAiguillage, (void *) NULL);
}

int lancerTrains(long nbr)
{
    long i;
    for(i=0;i<nbr;i++)
    {
        pthread_create(&pidTrains[i], 0, (void *(*)(void *))fonc_train, (void*) &i);
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

void *fonc_train(int* p)
{
    Train_t train = initTrainAiguillage(*p);
    //printf("Création de : ");
    printTrain(&train);
    while(!arrive(&train))
    {
        if(avance(&train) != 0)
        {
            //bloquerTrain
            printf("BLOQUE\n");
            exit(1);
        }
    }
    printf("Train_t %d terminé\n", train.id);
    pthread_exit(0);
}

void erreur(const char *str, int lvl)
{
    perror(str);
    exit(lvl);
}
