
#include "aiguillage.h"

extern Aiguillage aiguillage;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t GarrageGarreTGV PTHREAD_CON_INITIALIZER;

//pthread_cond_t conditions[4][2][3];

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
    int i;
    aiguillage.gare = initGare();
    aiguillage.garrage = initGarrage();
    aiguillage.tunnel = initTunnel();
    aiguillage.ligne = initLigne();
    //for(i=0;i<4*2*3;i++)
    //    (conditions+i) = PTHREAD_COND_INITIALIZER;
}

void demandeAcces(Train *train)
{
    pthread_mutex_lock(&mutex);
    int next;
    next = train->position + suivant(train);
    if(next == POS_GARE)
        demandeAccesGare(train);
    else if(next == POS_GARRAGE)
        demandeAccesGarrage(train);
    else if(next == POS_TUNNEL)
        demandeAccesTunnel(train);
    else if(next == POS_LIGNE)
        demandeAccesLigne(train);
    pthread_mutex_unlock(&mutex);
}

void demandeAccesGare(Train* train)
{

}

void demandeAccesGarrage(Train* train)
{

}

void demandeAccesTunnel(Train* train)
{

}

void demandeAccesLigne(Train* train)
{

}

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
    if(--aiguillage.ligne.voie.nbact == 0)
        aiguillage.ligne.voie.sens = BIDIRR;
}











