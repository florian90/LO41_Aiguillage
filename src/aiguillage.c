
#include "aiguillage.h"

Aiguillage_t aiguillage;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern Train_t trains[NB_THREAD];

/****************************************************************/
/************************* INITALISATION ************************/
/****************************************************************/

#define NBR_TRAINS_INIT 10
Train_t trainsInit[10] = {  {0, M   , OUEST   },
                            {1, TGV , EST   },
                            {2, GL  , OUEST },
                            {3, M   , EST   },
                            {4, GL  , OUEST },
                            {5, TGV , EST   },
                            {6, GL  , OUEST },
                            {7, M   , EST   },
                            {8, TGV , OUEST },
                            {9, M   , EST   },
                        };

Train_t initTrainAiguillage(int no)
{
    Train_t train;
    if(no < NBR_TRAINS_INIT)
    {
        train = trainsInit[no];
    }
    else
    {
        train = initRandTrain(no);
    }
    train.id = no;
    if(train.direction == EST)
    {
        train.position = POS_OUEST;
        utiliserVoie(&aiguillage.ouest.voieEst, train.position);
    }
    else
    {
        train.position = POS_EST;
        utiliserVoie(&aiguillage.ouest.voieOuest, train.position);
    }
    return train;
}

Sortie_t initOuest()
{
    Sortie_t o;
    o.voieEst = initVoie(EST, VAL_TOT, true, nextVoieOuest);
    o.voieOuest = initVoie(OUEST, VAL_TOT, true, nextNull);
    return o;
}

Gare_t initGare()
{
    Gare_t g;
    g.voieA = initCapa(EST ,VAL_M, NBR_STOCK, true, nextVoieGareA);
    g.voieB = initCapa(OUEST, VAL_M, NBR_STOCK, false, nextNull);
    g.voieC = initCapa(EST, VAL_TGV | VAL_GL, NBR_STOCK, true, nextVoieGareC);
    g.voieD = initCapa(OUEST, VAL_TGV | VAL_GL, NBR_STOCK, false, nextNull);
    return g;
}

Garage_t initGarage()
{
    Garage_t g;
    g.voieTGV = initCapa(BIDIRR, VAL_TGV, NBR_STOCK, true, nextVoieGarageTGV);
    g.voieMO  = initCapa(OUEST, VAL_M, NBR_STOCK, true, nextVoieGarageMO);
    g.voieME  = initCapa(EST, VAL_M, NBR_STOCK, true, nextVoieGarageME);;
    g.voieGL  = initCapa(BIDIRR, VAL_GL, NBR_STOCK, true, nextVoieGarageGL);
    return g;
}

Tunnel_t initTunnel()
{
    Tunnel_t t;
    t.voie = initCapa(BIDIRR, VAL_TOT, 1, false, nextVoieTunnel);
    return t;
}

Ligne_t initLigne()
{
    Ligne_t l;
    l.voie = initVoie(BIDIRR, VAL_TOT, false, nextVoieLigne);
    return l;
}

Sortie_t initEst()
{
    Sortie_t e;
    e.voieEst = initVoie(EST, VAL_TOT, true, nextNull);
    e.voieOuest = initVoie(OUEST, VAL_TOT, true, nextVoieEst);
    return e;
}

/*
 * Initialise toutes les parties de l'aiguillage
 */
void *initAiguillage(void *p)
{
    int priorite;
    aiguillage.ouest = initOuest();
    aiguillage.gare = initGare();
    aiguillage.garage = initGarage();
    aiguillage.tunnel = initTunnel();
    aiguillage.ligne = initLigne();
    aiguillage.est = initEst();
    aiguillage.update = 0;

    priorite = 1;
    while(1)
    {
        //dort(500000);
        liberePriorite(priorite);
        if(++priorite > 3)
            priorite = 1;
    }
    return NULL;
}

void liberePriorite(int priorite)
{
    Voie_t *voie;

    voie = &aiguillage.ouest.voieEst;
    if(voie->priorite == priorite)
    {
        // if(voieME.priorite < voieME.nbMax) ????
        printf("Test0\n");
        pthread_cond_broadcast(&voie->condition);
        resetPriorite(voie);
    }

    voie = &aiguillage.gare.voieA;
    if(voie->priorite == priorite)
    {
        // if(voieME.priorite < voieME.nbMax) ????
        printf("Test1\n");
        pthread_cond_broadcast(&voie->condition);
        resetPriorite(voie);
    }

    voie = &aiguillage.gare.voieC;
    if(voie->priorite == priorite)
    {
        if(peutUtiliser(&aiguillage.tunnel.voie, EST) && peutUtiliser(&aiguillage.ligne.voie, EST))
        { // Si on peut continuer après le tunnel
            if(voie->priorite == 1)
            {// TGV || GL
                if(peutUtiliser(&aiguillage.garage.voieTGV, EST) && peutUtiliser(&aiguillage.garage.voieGL, EST))
                {
                    printf("Test2\n");
                    pthread_cond_broadcast(&voie->condition);
                    resetPriorite(voie);
                }
            }
            else if(voie->priorite == 2)
            { // GL uniquement
                if(peutUtiliser(&aiguillage.garage.voieGL, EST))
                {
                    printf("Test3\n");
                    pthread_cond_broadcast(&voie->condition);
                    resetPriorite(voie);
                }
            }
        }
    }
    int i;
    voie = &aiguillage.garage.voieTGV;
    for(i=0;i<2;i++)
    {
        if(voie->priorite == priorite)
        {
            if(voie->sensAct == EST)
            {
                if(peutUtiliser(&aiguillage.tunnel.voie, EST) && peutUtiliser(&aiguillage.ligne.voie, EST))
                { // Si on peut continuer après le tunnel
                    printf("Test4\n");
                    pthread_cond_broadcast(&voie->condition);
                    resetPriorite(voie);
                }
            }
            else // OUEST
            {
                printf("Test5\n");
                pthread_cond_broadcast(&voie->condition);
                resetPriorite(voie);
            }
        }
        voie = &aiguillage.garage.voieGL;
    }
}

/******************************************************************/
/********* Fonctions à mettre dans les pointeurs des voies ********/
/******************************************************************/

Voie_t *nextNull(Direction d, Type t)
{
    return NULL;
}

/*               EST            */
Voie_t *nextVoieOuest(Direction d, Type t)
{
    if(t == M)
        return &aiguillage.gare.voieA;
    else // TGV | GL
        return &aiguillage.gare.voieC;
}

/*               GARE            */
Voie_t *nextVoieGareA(Direction d, Type t)
{
    if(d == OUEST)
    {
        printf("VOIE INTEXISTANTE");
        exit(-2);
    }
    return &aiguillage.garage.voieME;
}

Voie_t *nextVoieGareC(Direction d, Type t)
{
    if(t == GL)
        return &aiguillage.garage.voieGL;
    else // TGV
        return &aiguillage.garage.voieTGV;
}

/*               GARAGE               */
Voie_t *nextVoieGarageTGV(Direction d, Type t)
{
    if(d == est)
        return &aiguillage.tunnel.voie;
    else // ouest
        return &aiguillage.gare.voieD;
}

Voie_t *nextVoieGarageGL(Direction d, Type t)
{
    return nextVoieGarageTGV(d, t);
}

Voie_t *nextVoieGarageMO(Direction d, Type t)
{
    return &aiguillage.gare.voieB;
}

Voie_t *nextVoieGarageME(Direction d, Type t)
{
    return &aiguillage.tunnel.voie;
}

/*              Tunnel              */
Voie_t *nextVoieTunnel(Direction d, Type t)
{
    if(d == est)
        return &aiguillage.ligne.voie;
    else // ouest
    {
        if(t == TGV)
            return &aiguillage.garage.voieTGV;
        else if(t == GL)
            return &aiguillage.garage.voieGL;
        else // M
            return &aiguillage.garage.voieMO;
    }
}

/*              Ligne              */
Voie_t *nextVoieLigne(Direction d, Type t)
{
    if(d == est)
        return NULL;
    else // ouest
        return &aiguillage.tunnel.voie;
}

/*              OUEST              */
Voie_t *nextVoieEst(Direction d, Type t)
{
    return &aiguillage.ligne.voie;
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

int avance(Train_t *train)
{
    // Si peut s'arréter
    //     S'arrète + attends de pouvoir partir
    // Réserve l'accès au suivant
    // Si il ne peut pas avancer :
    //     retourne -1 => INTERBLOCAGE
    // avancer
    // Libère la voie précédente

    Voie_t *next;
    next = nextVoie(train);
    if(canStop(train->voie))
    {
        if(train->voie != NULL)
            setPrio(train->voie, train->type);
        else
            setPrio(&aiguillage.voieEst, train->type);
        // attends libération du suivant
        printf("Attends\n");
        pthread_cond_wait(&train->voie->condition, &mutex);
    }

    if(!utiliserVoie(next, train->direction))
    {
        // INTERBLOCAGE
        printf("ici : %d\n", train->position);
        return -1;
    }
    dort(TEMPS);
    printf("Train %d: %d->%d\n", (int) train->id, train->position-suivant(train), train->position);
    libererVoie(train->voie);

    train->voie = next;
    train->position += suivant(train);
    //getchar();
    printAiguillage();

    // printf("Train_t %d: sort de %d\n", (int) train->id, train->position-suivant(train));
    return 0;
}

Voie_t *nextVoie(Train_t *train)
{
    if(train != NULL && train->voie != NULL)
        return (*train->voie->fnNextVoie)(train->direction, train->type);
    else
    {
        if(train->direction == EST)
        {
            if(train->type == M)
                return &aiguillage.gare.voieA;
            else
                return &aiguillage.gare.voieC;
        }
        else // OUEST
        {
            return &aiguillage.ligne.voie;
        }
    }
}

/*
 * Affiche les détails de l'aiguillage
 */
void printAiguillage()
{
    printf("Garre   : %d\t%d\t%d\t%d\n", aiguillage.gare.voieA.nbAct, aiguillage.gare.voieB.nbAct, aiguillage.gare.voieC.nbAct, aiguillage.gare.voieD.nbAct);
    printf("Garage_t : %d\t%d\t%d\t%d\n", aiguillage.garage.voieGL.nbAct, aiguillage.garage.voieME.nbAct, aiguillage.garage.voieMO.nbAct, aiguillage.garage.voieTGV.nbAct);
    printf("Tunnel_t  : %d\n", aiguillage.tunnel.voie.nbAct);
    printf("Ligne_t   : %d\n\n", aiguillage.ligne.voie.nbAct);
}

void dort(int i)
{
    long j = 1000*(long) i;
    struct timespec t;
    t.tv_sec  = j/1000000000L;
    t.tv_nsec = j%1000000000L;
    nanosleep(&t, NULL);
}
