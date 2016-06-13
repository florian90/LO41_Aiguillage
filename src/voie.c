#include "voie.h"

/*
* Crée une nouvelle voie avec :
*      sens : un sens de circulation,
*      acces : un type de trains ayant accces
*      capacite un nombre de trains pouvant emprunter la voie en même temps (-1 si infini)
*/
Voie_t initCapa(Direction sens, Type acces, int capacite, bool canStop)
{
    Voie_t voie;
    voie.sens = voie.sensAct = sens;
    voie.acces = acces;
    voie.nbAct = 0;
    voie.nbMax = capacite;
    voie.canStop = canStop;
    pthread_cond_init(&voie.condition, NULL);
    voie.priorite = 0;
    return voie;
}

/*
* Crée une nouvelle voie avec :
*      sens : un sens de circulation,
*      acces : un type de trains ayant accces
*/
Voie_t initVoie(Direction sens, Type acces, bool canStop)
{
    return initCapa(sens, acces, -1, canStop);
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
bool utiliserVoie(Voie_t *voie, Direction d)
{
    // La voie n'est autorisée plus que dans cette direction
    voie->sensAct &= d;

    // Augmente le nombre de personnes sur la voie
    voie->nbAct++;

    // Retourne vraie si la voie peut être utilisée dans le sens direction
    // Faux sinon
    return peutUtiliser(voie, d);
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

bool attends(Voie_t *voie, Type t)
{
    return (voie->nbAttente[numTabType(t)] == 0);
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
