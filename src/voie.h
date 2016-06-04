#ifndef VOIE_H_INCLUDED
#define VOIE_H_INCLUDED

#include "structures.h"
#include "train.h"

typedef struct{
    Direction_t sens;
    Direction_t sensAct;
    Type_t acces;
    int nbAct;
    int nbMax;
} Voie_t;

Voie_t initVoie(int sens, int acces);
Voie_t initCapa(Direction_t sens, Type_t acces, int capacite);

void utiliserVoie(Train_t *train, Voie_t *voie);
void libererVoie(Train_t *train, Voie_t *voie);

int peutAcceder(Train_t *train, Voie_t *voie);

#endif // VOIE_H_INCLUDED
