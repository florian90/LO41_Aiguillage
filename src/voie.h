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
	pthread_mutex_t mutex;
	int nbAttente[3];
	pthread_cond_t condition[3];
	int (*funcLiberer)(int priorite);
} Voie_t;

Voie_t initVoie(Direction sens, Type acces, bool canStop);
Voie_t initCapa(Direction sens, Type acces, int capacite, bool canStop);
Voie_t


bool peutUtiliser(Voie_t *voie, Direction d);
bool utiliserVoie(Voie_t *voie, Direction d);
void libererVoie(Voie_t *voie);

bool canStop(Voie_t *voie);

void ajouterAttente(Voie_t *voie, Type t);
void retirerAttente(Voie_t *voie, Type t);
bool attends(Voie_t *voie, Type t);

int numTabType(Type t);

#endif // VOIE_H_INCLUDED
