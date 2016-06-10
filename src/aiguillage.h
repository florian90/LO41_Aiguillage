#ifndef AIGUILLAGE_H_INCLUDED
#define AIGUILLAGE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "train.h"
#include "structures.h"
#include "voie.h"
#include "list.h"

#define NBR_STOCK 10

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

    int update;
}Aiguillage_t;


Train_t initTrainAiguillage(int no);

Gare_t initGare();
Garage_t initGarage();
Tunnel_t initTunnel();
Ligne_t initLigne();

void *initAiguillage(void *p);

void liberePriorite(int priorite);

int avance(Train_t *train);

void printAiguillage();

Voie_t *nextNull(Direction d, Type t);
Voie_t *nextVoieOuest(Direction d, Type t);
Voie_t *nextVoieGareA(Direction d, Type t);
Voie_t *nextVoieGareC(Direction d, Type t);
Voie_t *nextVoieGarageTGV(Direction d, Type t);
Voie_t *nextVoieGarageGL(Direction d, Type t);
Voie_t *nextVoieGarageMO(Direction d, Type t);
Voie_t *nextVoieGarageME(Direction d, Type t);
Voie_t *nextVoieTunnel(Direction d, Type t);
Voie_t *nextVoieLigne(Direction d, Type t);
Voie_t *nextVoieEst(Direction d, Type t);

Voie_t *nextVoie(Train_t *train);

void dort(int i);

#endif // AIGUILLAGE_H_INCLUDED
