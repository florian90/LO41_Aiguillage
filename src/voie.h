#ifndef VOIE_H_INCLUDED
#define VOIE_H_INCLUDED

#include <pthread.h>
#include <stdbool.h>

#include "structures.h"

typedef struct Voie_s {
    Direction sens;
    Direction sensAct;
    Type acces;
    int nbAct;
    int nbMax;
    bool canStop;
    pthread_cond_t condition;
    struct Voie_s *(*fnNextVoie)(Direction, Type);
    int priorite;
} Voie_t;

Voie_t initVoie(Direction sens, Type acces, bool canStop, Voie_t *(*fnc)(Direction, Type ));
Voie_t initCapa(Direction sens, Type acces, int capacite, bool canStop, Voie_t *(*fnc)(Direction, Type ));

bool peutUtiliser(Voie_t *voie, Direction d);
bool utiliserVoie(Voie_t *voie, Direction d);
void libererVoie(Voie_t *voie);

bool canStop(Voie_t *voie);

void setPrio(Voie_t *voie, Type t);
void resetPriorite(Voie_t *voie);

#endif // VOIE_H_INCLUDED
