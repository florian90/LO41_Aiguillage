#include "voie.h"

/*
 * Crée une nouvelle voie avec :
 *      sens : un sens de circulation,
 *      acces : un type de trains ayant accces
 *      capacite un nombre de trains pouvant emprunter la voie en même temps (-1 si infini)
 */
Voie_t initCapa(Direction sens, Type acces, int capacite, bool canStop, Voie_t *(*fnc)(Direction, Type ))
{
    Voie_t voie;
    voie.sens = voie.sensAct = sens;
    voie.acces = acces;
    voie.nbAct = 0;
    voie.nbMax = capacite;
    voie.canStop = canStop;
    pthread_cond_init(&voie.condition, NULL);
    voie.fnNextVoie = fnc;
    voie.priorite = 0;
    return voie;
}

/*
 * Crée une nouvelle voie avec :
 *      sens : un sens de circulation,
 *      acces : un type de trains ayant accces
 */
Voie_t initVoie(Direction sens, Type acces, bool canStop, Voie_t *(*fnc)(Direction, Type ))
{
    return initCapa(sens, acces, -1, canStop, fnc);
}

bool peutUtiliser(Voie_t *voie, Direction d)
{
    return ((voie->sensAct & d) != 0);
}

/*
 * La voie ne pourra plus que foncitonner dans le sens donné
 * Si la voie ne peut pas être utilisée dans le sens donné,
 *     retourne false, sinon retourne true
 * d : Direction : soit EST soit OUEST, pas BIDIRR
 */
bool utiliserVoie(Voie_t *voie, Direction d)
{
    // Si voie peut fonctionner dans la direction donnée :
    if((voie->sens & d) != 0)
    {
        // La voie n'est autorisée plus que dans cette direction
        voie->sensAct &= d;
    }
    // Augmente le nombre de personnes sur la voie
    voie->nbAct++;
    // Si la voie est sur utilisée :
    // if(voie->nbMax != -1 && voie->nbAct < voie->nbMax)
    //     return false;

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
    if(voie != NULL && --voie->nbAct == 0)
        voie->sensAct = voie->sens;
}

bool canStop(Voie_t *voie)
{
    if(voie == NULL)
        return false; ///////::
    return voie->canStop;
}

void setPrio(Voie_t *voie, Type t)
{
    int p = getPriorite(t);
    if(voie->priorite == 0)
        voie->priorite = p;
    else if(voie->priorite > p)
        voie->priorite = p;
}

void resetPriorite(Voie_t *voie)
{
    voie->priorite = 0;
}
