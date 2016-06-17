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

/*
 * Crée une nouvelle voie sur laquelle on peut s'arreter avec :
 *      sens : un sens de circulation,
 *      acces : un type de trains ayant accces
 *      capacite : un nombre de trains pouvant emprunter la voie en même temps (-1 si infini)
 *		funcLiberer : Une fonction permettant de libérer un train de la voie si les conditions le permettent
*/
void initArret(Voie_t *voie, Direction sens, Type acces, int capacite, void (*funcLiberer)(Type t))
{
	_initVoie(voie, sens, acces, capacite, true, funcLiberer);
}

bool peutUtiliser(Voie_t *voie, Direction d)
{
	return ((voie->sensAct & d) != 0) && (voie->nbMax == -1 || voie->nbAct <= voie->nbMax);
}

bool peutAjouterTrain(Voie_t *voie, Direction d)
{
	return ((voie->sensAct & d) != 0) && (voie->nbMax == -1 || voie->nbAct < voie->nbMax);
}

/*
 * La voie ne pourra plus que foncitonner dans le sens donné
 * Si la voie ne peut pas être utilisée dans le sens donné,
 *     retourne false, sinon retourne true
 * d : Direction : soit EST soit OUEST, pas BIDIRR
*/
void utiliserVoie(Voie_t *voie, Direction d)
{
    // La voie n'est disponible plus que dans cette direction
	voie->sensAct &= d;

    // Augmente le nombre de trains sur la voie
	voie->nbAct++;
}

/*
 * Retire un train de la voie, libère le sens si il n'y a plus de trains dessus
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

/*
 * Retourne vraie si un train avec une priorité suppéreure est en attente
*/
bool supEnAttente(Voie_t *voie, Type t)
{
	switch (t) {
		case M	:
			if(enAttente(voie, GL))
				return true;
		case GL	:
			if(enAttente(voie, TGV))
				return true;
	}
	return false;
}

/*
 * Retourne vraie si  :
 *	le type t est prioritaire sur les trains de la voie
 * 	et que les trains de la voie attendent dans la direction sens
*/
bool estPrioritaireSur(Type t, Voie_t *voie, Direction sens)
{
	return !supEnAttente(voie, t) || (voie->sensAct == sens);
}

/*
 * Retourne l'indice du tableau pour les conditions 
 * 	et le nombre de trains en attenteen fonction du type
*/
int numTabType(Type t)
{
	switch (t) {
		case TGV : return 0;
		case GL  : return 1;
		case M   : return 2;
	}
	return -1;
}
