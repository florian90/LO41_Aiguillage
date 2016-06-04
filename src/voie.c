#include "voie.h"

/*
 * Crée une nouvelle voie avec :
 *      sens : un sens de circulation,
 *      acces : un type de trains ayant accces
 *      capacite un nombre de trains pouvant emprunter la voie en même temps (-1 si infini)
 */
Voie_t initCapa(Direction_t sens, Type_t acces, int capacite)
{
    Voie_t voie={sens, sens, acces, 0, capacite};
    return voie;
}

/*
 * Crée une nouvelle voie avec :
 *      sens : un sens de circulation,
 *      acces : un type de trains ayant accces
 */
Voie_t initVoie(int sens, int acces)
{
    return initCapa(sens, acces, -1);
}

/*
 * Retourne vraie si le train peut emprunter la voie, faux sinon
 */
int peutAcceder(Train_t *train, Voie_t *voie)
{
//La voie peut contenir des trains dans la direction du train
//      ET (la voie a une capacité infinie OU la capacité maximale n'est pas atteinte)
    return (voie->sens&train->direction) && (voie->nbMax == -1 || voie->nbAct < voie->nbMax);
}

/*
 * Ajoute un train sur une voie
 * La voie ne peut plus être utilisée dans la direction opposée au train
 */
void utiliserVoie(Train_t *train, Voie_t *voie)
{
    voie->nbAct++;
    voie->sensAct = train->direction;
}

/*
 * Retire un train sur une voie
 * Si la voie est vide, elle peut de nouveau être
 *   utilisée dans tous les sens prévus initaliement
 */
void libererVoie(Train_t *train, Voie_t *voie)
{
    if(--voie->nbAct == 0)
        voie->sensAct = voie->sens;
}
