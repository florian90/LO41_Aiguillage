
#include "aiguillage.h"

extern Aiguillage aiguillage;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t conditions[4][2][3];
pthread_cond_t condTunnelOccupe = PTHREAD_COND_INITIALIZER;

Voie initVoie(int sens, int acces)
{
    Voie voie={sens, sens, acces, 0, -1};
    return voie;
}

Voie initCapa(int sens, int acces, int capacite)
{
    Voie voie={sens, sens, acces, 0, capacite};
    return voie;
}

Gare initGare()
{
    Gare g;
    g.voieA = initCapa(EST ,VAL_M, NBR_STOCK);
    g.voieB = initCapa(OUEST, VAL_M, NBR_STOCK);
    g.voieC = initCapa(EST, VAL_TGV | VAL_GL, NBR_STOCK);
    g.voieD = initCapa(OUEST, VAL_TGV | VAL_GL, NBR_STOCK);
    return g;
}

Garage initGarage()
{
    Garage g;
    g.voieTGV = initCapa(BIDIRR, VAL_TGV, NBR_STOCK);
    g.voieMO  = initCapa(OUEST, VAL_M, NBR_STOCK);
    g.voieME  = initCapa(EST, VAL_M, NBR_STOCK);;
    g.voieGL  = initCapa(BIDIRR, VAL_GL, NBR_STOCK);
    return g;
}

Tunnel initTunnel()
{
    Tunnel t;
    t.voie = initCapa(BIDIRR, VAL_TOT, 1);
    pthread_cond_signal(&condTunnelOccupe);
    return t;
}

Ligne initLigne()
{
    Ligne l;
    l.voie = initVoie(BIDIRR, VAL_TOT);
    return l;
}

void initAiguillage()
{
    aiguillage.gare = initGare();
    aiguillage.garage = initGarage();
    aiguillage.tunnel = initTunnel();
    aiguillage.ligne = initLigne();
}

/*
 * Prends les ressources en prévision de les utiliser plus tard
 */
void demandeAcces(Train *train)
{
    int next;
    pthread_mutex_lock(&mutex);
    next = train->position + suivant(train);
    if(next == POS_GARE)
        accederGare(train);
    else if(next == POS_GARRAGE)
        accederGarage(train);
    else if(next == POS_TUNNEL)
        accederTunnel(train);
    else if(next == POS_LIGNE)
        accederLigne(train);
    printAiguillage();
    pthread_mutex_unlock(&mutex);
}

/*
 * Ajoute un train sur une voie
 * La voie ne peut plus être utilisée dans la direction opposée
 */
void utiliserVoie(Train *train, Voie *voie)
{
    voie->nbAct++;
    voie->sensAct = train->direction;
}

/*
 * Retire un train sur une voie
 * Si la voie est vide, elle peut de nouveau être
 *   utilisée dans tous les sens prévus initaliement
 */
void libererVoie(Train *train, Voie *voie)
{
    if(--voie->nbAct == 0)
        voie->sensAct = voie->sens;
}

void accederGare(Train* train)
{
    if(train->direction == EST)
    {
        if(train->type == M)
        {
            utiliserVoie(train, &aiguillage.gare.voieA);
        }
        else
        {
            utiliserVoie(train, &aiguillage.gare.voieC);
        }
    }
    else // direction == OUEST
    {
        if(train->type == M)
        {
            utiliserVoie(train, &aiguillage.gare.voieB);
        }
        else
        {
            utiliserVoie(train, &aiguillage.gare.voieD);
        }
    }
}

void accederGarage(Train* train)
{
    if(train->type == TGV)
    {
        utiliserVoie(train, &aiguillage.garage.voieTGV);
    }
    else if (train->type == GL)
    {
        utiliserVoie(train, &aiguillage.garage.voieGL);
    }
    else // train->type == M
    {
        if(train->direction == EST)
        {
            utiliserVoie(train, &aiguillage.garage.voieME);
        }
        else
        {
            utiliserVoie(train, &aiguillage.garage.voieMO);
        }
    }
}

/*
 * Retourne vraie si le train peut emprunter la voie, faux sinon
 */
int peutAcceder(Train *train, Voie *voie)
{
// Le train peut partir dans cette direction
//      ET (la voie a une capacité infinie OU la capacité maximale n'est pas atteinte)
    return (voie->sens&train->direction) && (voie->nbMax == -1 || voie->nbAct < voie->nbMax);
}

void accederTunnel(Train* train)
{
    if(!peutAcceder(train, &aiguillage.tunnel.voie))
    {
        puts("test");
        pthread_cond_wait(&condTunnelOccupe, &mutex);
    }
    utiliserVoie(train, &aiguillage.tunnel.voie);
}

void accederLigne(Train* train)
{
    utiliserVoie(train, &aiguillage.ligne.voie);
}

/*
 * Libère tous les accès lorsque le train quitte
 * un morceau de l'aiguillage
 */
void libererAcces(Train *train)
{
    int prev;
    pthread_mutex_lock(&mutex);
    prev = train->position - suivant(train);
    if(prev == POS_GARE)
        libererAccesGare(train);
    else if(prev == POS_GARRAGE)
        libererAccesGarage(train);
    else if(prev == POS_TUNNEL)
        libererAccesTunnel(train);
    else if(prev == POS_LIGNE)
        libererAccesLigne(train);
    printAiguillage();
    pthread_mutex_unlock(&mutex);
}

void libererAccesGare(Train *train)
{
    if(train->type == M)
    {
        if(train->direction == EST)
            libererVoie(train, &aiguillage.gare.voieA);
        else
            libererVoie(train, &aiguillage.gare.voieB);
    }
    else // TGV || GL
    {
        if(train->direction == EST)
            libererVoie(train, &aiguillage.gare.voieC);
        else
            libererVoie(train, &aiguillage.gare.voieD);
    }
}

void libererAccesGarage(Train *train)
{
    if(train->type == M)
    {
        if(train->direction == OUEST)
            libererVoie(train, &aiguillage.garage.voieMO);
        else
            libererVoie(train, &aiguillage.garage.voieME);
    }
    else if(train->type == TGV)
    {
        libererVoie(train, &aiguillage.garage.voieTGV);
    }
    else // GL
    {
        libererVoie(train, &aiguillage.garage.voieGL);
    }
}

void libererAccesTunnel(Train *train)
{
    libererVoie(train, &aiguillage.tunnel.voie);
    if(aiguillage.tunnel.voie.nbAct == 0)
        pthread_cond_signal(&condTunnelOccupe);
}

void libererAccesLigne(Train *train)
{
    // Retire le train de la voie
    libererVoie(train, &aiguillage.ligne.voie);
    // Libère les ressources qui peuvent l'être
}

/*
 * Affiche les détails de l'aiguillage
 */
void printAiguillage()
{
    printf("Garre   : %d\t%d\t%d\t%d\n", aiguillage.gare.voieA.nbAct, aiguillage.gare.voieB.nbAct, aiguillage.gare.voieC.nbAct, aiguillage.gare.voieD.nbAct);
    printf("Garage : %d\t%d\t%d\t%d\n", aiguillage.garage.voieGL.nbAct, aiguillage.garage.voieME.nbAct, aiguillage.garage.voieMO.nbAct, aiguillage.garage.voieTGV.nbAct);
    printf("Tunnel  : %d\n", aiguillage.tunnel.voie.nbAct);
    printf("Ligne   : %d\n\n", aiguillage.ligne.voie.nbAct);
}
