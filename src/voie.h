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
	void (*funcLiberer)(Type t);

} Voie_t;

void initVoie(Voie_t *voie, Direction sens, Type acces);
void initCapa(Voie_t *voie, Direction sens, Type acces, int capacite);
void initArret(Voie_t *voie, Direction sens, Type acces, int capacite, void (*funcLiberer)(Type t));

bool peutUtiliser(Voie_t *voie, Direction d);
bool peutAjouterTrain(Voie_t *voie, Direction d);
void utiliserVoie(Voie_t *voie, Direction d);
void libererVoie(Voie_t *voie);

bool canStop(Voie_t *voie);

void ajouterAttente(Voie_t *voie, Type t);
void retirerAttente(Voie_t *voie, Type t);

bool enAttente(Voie_t *voie, Type t);
bool supEnAttente(Voie_t *voie, Type t);
bool estPrioritaireSur(Type t, Voie_t *voie, Direction sens);

int numTabType(Type t);

#endif // VOIE_H_INCLUDED
