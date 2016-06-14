#ifndef AIGUILLAGE_H_INCLUDED
#define AIGUILLAGE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "train.h"
#include "structures.h"
#include "voie.h"
#include "list.h"

extern pthread_mutex_t mutexEcriture;
extern pthread_mutex_t mutexAiguillagePret;

extern pthread_cond_t condAiguillagePret;

extern Train_t trains[NB_THREAD];

typedef struct {
	Voie_t voieA;
	Voie_t voieB;
	Voie_t voieC;
	Voie_t voieD;
}Gare_t;

typedef struct {
	Voie_t voieTGV;
	Voie_t voieMO;
	Voie_t voieME;
	Voie_t voieGL;
}Garage_t;

typedef struct {
	Voie_t voie;
}Tunnel_t;

typedef struct {
	Voie_t voie;
}Ligne_t;

typedef struct {
	Voie_t voieEst;
	Voie_t voieOuest;
} Sortie_t;

typedef struct {
	Sortie_t ouest;
	Gare_t gare;
	Garage_t garage;
	Tunnel_t tunnel;
	Ligne_t ligne;
	Voie_t voieEst;
	Sortie_t est;

	volatile int update;
}Aiguillage_t;

Train_t initTrainAiguillage(int no);

void initEst();
void initGare();
void initGarage();
void initTunnel();
void initLigne();
void initOuest();

void *initAiguillage(void *p);

int liberePriorite(Type t);

int avance(Train_t *train);

void printAiguillage();

Voie_t *getVoie(int position, Type type, Direction direction);
Voie_t *actVoie(Train_t *train);
Voie_t *nextVoie(Train_t *train);

void dort(int i);

int libererOuest(Type t);
int libererGareA(Type t);
int libererGareC(Type t);
int libererGarageTGV(Type t);
int libererGarageMO(Type t);
int libererGarageME(Type t);
int libererGarageGL(Type t);
int libererEst(Type t);

#endif // AIGUILLAGE_H_INCLUDED
