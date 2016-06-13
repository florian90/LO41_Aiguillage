#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "structures.h"
#include "train.h"
#include "aiguillage.h"

void lancerAiguillage();
void *fonc_train(void *p);
int lancerTrains();
void erreur(const char *str, int lvl);

pthread_mutex_t mutexEcriture = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexAiguillagePret = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condAiguillagePret = PTHREAD_COND_INITIALIZER;

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

	lancerTrains(NB_THREAD);
	return 0;
}

void lancerAiguillage()
{
	pthread_create(&pidAiguilleur, 0, (void *(*)(void *))initAiguillage, (void *) NULL);
}

int lancerTrains(long nbr)
{
	pthread_cond_wait(&condAiguillagePret, &mutexAiguillagePret);
	long i;
	for(i=0;i<nbr;i++)
	{
		pthread_create(&pidTrains[i], 0, (void *(*)(void *))fonc_train, (void*) i);
		//usleep(TEMPS);
	}
	for(i=0;i<nbr;i++)
	{
		pthread_join(pidTrains[i], NULL);
	}
	return 0;
}

void *fonc_train(void *p)
{
	int idx = (int) p;
	trains[idx] = initTrainAiguillage(idx);
	while(!arrive(&trains[idx]))
	{
		if(avance(&trains[idx]) != 0)
		{
			//bloquerTrain
			safePuts("BLOQUE\n");
			exit(1);
		}
	}
	pthread_exit(0);
}

void erreur(const char *str, int lvl)
{
	perror(str);
	exit(lvl);
}
