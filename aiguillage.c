
#include "aiguillage.h"

extern Aiguillage aiguillage;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t conditions[4][2][3];

Voie initVoie(int sens, int acces)
{
    Voie voie={sens, acces, 0, 1};
    return voie;
}

Voie initStock(int sens, int acces, int capacite)
{
    Voie voie={sens, acces, 0, capacite};
    return voie;
}

Gare initGare()
{
    Gare g;
    g.voieA = initStock(EST ,VAL_M, NBR_STOCK);
    g.voieB = initStock(OUEST, VAL_M, NBR_STOCK);
    g.voieC = initStock(EST, VAL_TGV | VAL_GL, NBR_STOCK);
    g.voieD = initStock(OUEST, VAL_TGV | VAL_GL, NBR_STOCK);
    return g;
}

Garrage initGarrage()
{
    Garrage g;
    g.voieTGV = initStock(BIDIRR, VAL_TGV, NBR_STOCK);
    g.voieMO  = initStock(OUEST, VAL_M, NBR_STOCK);
    g.voieME  = initStock(EST, VAL_M, NBR_STOCK);;
    g.voieGL  = initStock(BIDIRR, VAL_GL, NBR_STOCK);
    return g;
}

Tunnel initTunnel()
{
    Tunnel t;
    t.voie = initVoie(BIDIRR, VAL_TOT);
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
    int i, j, k;
    aiguillage.gare = initGare();
    aiguillage.garrage = initGarrage();
    aiguillage.tunnel = initTunnel();
    aiguillage.ligne = initLigne();
    for(i=0;i<4;i++)
        for(j=0;j<2;j++)
            for(k=0;k<3;k++)
            {
                pthread_mutex_init(&conditions[i][j][k], NULL);
                pthread_cond_signal(&conditions[i][j][k]);
            }
}

void demanderAcces(Train *train)
{

    acceder(&train);
    printAiguillage();
    pthread_mutex_unlock(&mutex);
}

/*
 * Prends les ressources en prévision de les utiliser plus tard
 */
void acceder(Train *train)
{
    int next;
    pthread_mutex_lock(&mutex);
    next = train->position + suivant(train);
    if(next == POS_GARE)
        accederGare(train);
    else if(next == POS_GARRAGE)
        accederGarrage(train);
    else if(next == POS_TUNNEL)
        accederTunnel(train);
    else if(next == POS_LIGNE)
        accederLigne(train);
    acceder(&train);
    printAiguillage();
    pthread_mutex_unlock(&mutex);
}

void accederGare(Train* train)
{
    if(train->direction == EST)
    {
        if(train->type == M)
        {
            aiguillage.gare.voieA.nbact++;
        }
        else
        {
            aiguillage.gare.voieC.nbact++;
        }
    }
    else // direction == OUEST
    {
        if(train->type == M)
        {
            aiguillage.gare.voieB.nbact++;
        }
        else
        {
            aiguillage.gare.voieD.nbact++;
        }
    }
}

void accederGarrage(Train* train)
{
    if(train->type == TGV)
    {
        aiguillage.garrage.voieTGV.sens = train->direction;
        aiguillage.garrage.voieTGV.nbact++;
    }
    else if (train->type == GL)
    {
        aiguillage.garrage.voieGL.sens = train->direction;
        aiguillage.garrage.voieGL.nbact++;
    }
    else // train->type == M
    {
        if(train->direction == EST)
        {
            aiguillage.garrage.voieME.nbact++;
        }
        else
        {
            aiguillage.garrage.voieMO.nbact++;
        }
    }
}

void accederTunnel(Train* train)
{
    aiguillage.tunnel.voie.nbact++;
    aiguillage.tunnel.voie.sens = train->direction;
}

void accederLigne(Train* train)
{
    aiguillage.ligne.voie.nbact++;
    aiguillage.ligne.voie.sens = train->direction;
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
        libererAccesGarrage(train);
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
            aiguillage.gare.voieA.nbact--;
        else
            aiguillage.gare.voieB.nbact--;
    }
    else // TGV || GL
    {
        if(train->direction == EST)
            aiguillage.gare.voieC.nbact--;
        else
            aiguillage.gare.voieD.nbact--;
    }
}

void libererAccesGarrage(Train *train)
{
    if(train->type == M)
    {
        if(train->direction == OUEST)
            aiguillage.garrage.voieMO.nbact--;
        else
            aiguillage.garrage.voieME.nbact--;
    }
    else if(train->type == TGV)
    {
        if(--aiguillage.garrage.voieTGV.nbact == 0)
            aiguillage.garrage.voieTGV.sens = BIDIRR;
    }
    else // GL
    {
        if(--aiguillage.garrage.voieGL.nbact == 0)
            aiguillage.garrage.voieGL.sens = BIDIRR;
    }
}

void libererAccesTunnel(Train *train)
{
    if(--aiguillage.tunnel.voie.nbact == 0)
        aiguillage.tunnel.voie.sens = BIDIRR;
}

void libererAccesLigne(Train *train)
{
    // Retire le train de la voie
    if(--aiguillage.ligne.voie.nbact == 0)
        aiguillage.ligne.voie.sens = BIDIRR;

    // Libère les ressources qui peuvent l'être

}

/*
 * Affiche les détails de l'aiguillage
 */
void printAiguillage()
{
    printf("Garre   : %d\t%d\t%d\t%d\n", aiguillage.gare.voieA.nbact, aiguillage.gare.voieB.nbact, aiguillage.gare.voieC.nbact, aiguillage.gare.voieD.nbact);
    printf("Garrage : %d\t%d\t%d\t%d\n", aiguillage.garrage.voieGL.nbact, aiguillage.garrage.voieME.nbact, aiguillage.garrage.voieMO.nbact, aiguillage.garrage.voieTGV.nbact);
    printf("Tunnel  : %d\n", aiguillage.tunnel.voie.nbact);
    printf("Ligne   : %d\n\n", aiguillage.ligne.voie.nbact);
}
