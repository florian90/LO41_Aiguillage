#include "voie.h"


void _initVoie(Voie_t *voie, Direction sens, Type acces, int capacite, bool canStop, void (*funcLiberer)(Type t))
{
	int i;
	voie->sens = voie->sensAct = sens;
	voie->acces = acces;
	voie->nbAct = 0;
	voie->nbMax = capacite;

	voie->canStop = canStop;
	pthread_mutex_init(&voie->mutex, NULL);
	for (i = 0; i < 3; ++i)
	{
		voie->nbAttente[i] = 0;
		pthread_cond_init(&voie->condition[i], NULL);
	}
	voie->funcLiberer = funcLiberer;
}

/*
 * Crée une nouvelle voie avec :
 *      sens : un sens de circulation,
 *      acces : un type de trains ayant accces
 *      capacite un nombre de trains pouvant emprunter la voie en même temps (-1 si infini)
*/
void initCapa(Voie_t *voie, Direction sens, Type acces, int capacite)
{
	_initVoie(voie, sens, acces, capacite, false, NULL);
}

/*
 * Crée une nouvelle voie avec :
 *      sens : un sens de circulation,
 *      acces : un type de trains ayant accces
*/
void initVoie(Voie_t *voie, Direction sens, Type acces)
{
	initCapa(voie, sens, acces, -1);
}

void initArret(Voie_t *voie, Direction sens, Type acces, int capacite, int (*funcLiberer)(Type t))
{
	_initVoie(voie, sens, acces, capacite, true, funcLiberer);
}

bool peutUtiliser(Voie_t *voie, Direction d)
{
	return ((voie->sensAct & d) != 0) && (voie->nbMax == -1 || voie->nbAct <= voie->nbMax);
}

/*
 * La voie ne pourra plus que foncitonner dans le sens donné
 * Si la voie ne peut pas être utilisée dans le sens donné,
 *     retourne false, sinon retourne true
 * d : Direction : soit EST soit OUEST, pas BIDIRR
*/
void utiliserVoie(Voie_t *voie, Direction d)
{
    // La voie n'est autorisée plus que dans cette direction
	voie->sensAct &= d;

    // Augmente le nombre de personnes sur la voie
	voie->nbAct++;
}

/*
 * La voie peut à nouveau fonctionner dans toutes les directions initalement prévus
*/
void libererVoie(Voie_t *voie)
{
// NULL si sur case départ
	if(--voie->nbAct == 0)
		voie->sensAct = voie->sens;
}

bool canStop(Voie_t *voie)
{
	return voie->canStop;
}

void ajouterAttente(Voie_t *voie, Type t)
{
	voie->nbAttente[numTabType(t)]++;
}

void retirerAttente(Voie_t *voie, Type t)
{
	voie->nbAttente[numTabType(t)]--;
}

bool enAttente(Voie_t *voie, Type t)
{
	return (voie->nbAttente[numTabType(t)] != 0);
}

int numTabType(Type t)
{
	switch (t) {
		case TGV : return 0;
		case GL  : return 1;
		case M   : return 2;
	}
	return -1;
}
