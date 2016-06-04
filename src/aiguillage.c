
#include "aiguillage.h"

Aiguillage_t aiguillage;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern Train_t trains[NB_THREAD];

/****************************************************************/
/************************* INITALISATION ************************/
/****************************************************************/

Gare_t initGare()
{
    Gare_t g;
    g.voieA = initCapa(EST ,VAL_M, NBR_STOCK);
    g.voieB = initCapa(OUEST, VAL_M, NBR_STOCK);
    g.voieC = initCapa(EST, VAL_TGV | VAL_GL, NBR_STOCK);
    g.voieD = initCapa(OUEST, VAL_TGV | VAL_GL, NBR_STOCK);
    return g;
}

Garage_t initGarage()
{
    Garage_t g;
    g.voieTGV = initCapa(BIDIRR, VAL_TGV, NBR_STOCK);
    g.voieMO  = initCapa(OUEST, VAL_M, NBR_STOCK);
    g.voieME  = initCapa(EST, VAL_M, NBR_STOCK);;
    g.voieGL  = initCapa(BIDIRR, VAL_GL, NBR_STOCK);
    return g;
}

Tunnel_t initTunnel()
{
    Tunnel_t t;
    t.voie = initCapa(BIDIRR, VAL_TOT, 1);
    return t;
}

Ligne_t initLigne()
{
    Ligne_t l;
    l.voie = initVoie(BIDIRR, VAL_TOT);
    return l;
}

/*
 * Initialise toutes les parties de l'aiguillage
 */
void *initAiguillage(void *p)
{
    aiguillage.gare = initGare();
    aiguillage.garage = initGarage();
    aiguillage.tunnel = initTunnel();
    aiguillage.ligne = initLigne();
    aiguillage.update = 0;
    while(1)
    {
        signalTrains();
    }
}






int avance(Train_t *train)
{
    // Si peut s'arréter
    //     S'arrète + attends de pouvoir partir
    // Réserve l'accès au suivant
    // Si il ne peut pas avancer :
    //     retourne -1 => INTERBLOCAGE
    // avancer
    // Libère la voie précédente


    // demande acces au suivant, OK au retour
    demandeAcces(train);

    // avance le train
    train->position += suivant(train);
    printf("Train_t %d: %d->%d\n", (int) train->id, train->position-suivant(train), train->position);

    usleep(TEMPS);

    // libere l'acces du précédent
    libererAcces(train);
    printf("Train_t %d: sort de %d\n", (int) train->id, train->position-suivant(train));
    return 0;
}




/****************************************************************/
/************************ DEMANDE ACCES *************************/
/****************************************************************/

/*
 * Prends les ressources en prévision de les utiliser plus tard
 */
void demandeAcces(Train_t *train)
{
    pthread_mutex_lock(&mutex);
    addToWaitList(train);
    //pthread_cond_wait(&train->condition, &mutex);
    pthread_mutex_unlock(&mutex);
}

void addToWaitList(Train_t *train)
{
    List_t *l;
    switch(train->type)
    {
        case TGV:
            l = &aiguillage.listTGV;
            break;
        case GL:
            l = &aiguillage.listGL;
            break;
        case M  :
            l = &aiguillage.listM;
            break;
    }
    aiguillage.update = 1;
    addList(l, train);
}

/****************************************************************/
/*********************** PRENDRE POSITION ***********************/
/****************************************************************/

void accederGare(Train_t* train)
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

void accederGarage(Train_t* train)
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

void Tunnel(Train_t *train)
{
    ;
}

void accederLigne(Train_t* train)
{
    if(train->direction == OUEST)
    {
        if(!peutAcceder(train, &aiguillage.tunnel.voie))
        {

        }
    }
    // si train->direction == EST => TOujours OK car est déjà dans le tunnel
    utiliserVoie(train, &aiguillage.ligne.voie);
}

/****************************************************************/
/********************** FREE THE TRAINS *************************/
/****************************************************************/

void signalTrains()
{
    // Test TGVs puis GL puis M
    // Si une modification est effectuée (libération ou accès)
    //   ->  recommence pour laisser la place aux TGVs

    if(!signalTrainList(&aiguillage.listTGV) ||
        !signalTrainList(&aiguillage.listGL) ||
        !signalTrainList(&aiguillage.listM))
    {
        ;
    }
}

int signalTrainList(List_t *list)
{
    struct Elem *act;
    act = *list;
    while(act != NULL)
    {
        if(aiguillage.update)
            return 1;
        signalTrain(act->train);
        act = act->next;
    }
    return 0;
}

void signalTrain(Train_t *train)
{
    printf("Train : %d", (int) train->id);
    pthread_cond_signal(&train->condition);
    return;
    if(train->direction == est)
    {
        if(train->position < POS_GARE)
        {
        printf("TEST\n");
            // OK
            pthread_cond_signal(&train->condition);
            // Réserver gare
            if(train->type == M)
                utiliserVoie(train, &aiguillage.gare.voieA);
            else
                utiliserVoie(train, &aiguillage.gare.voieC);
        }
        else if(train->position == POS_GARE)
        {
            // Réserver garage
            if(train->type == M)
                utiliserVoie(train, &aiguillage.garage.voieME);
            else if(train->type == GL)
                utiliserVoie(train, &aiguillage.garage.voieGL);
            else
                utiliserVoie(train, &aiguillage.garage.voieTGV);
        }
        else if(train->position == POS_GARAGE)
        {
            // Réserver tunnel + ligne
            utiliserVoie(train, &aiguillage.tunnel.voie);
            utiliserVoie(train, &aiguillage.ligne.voie);
        }
        else if(train->position == POS_TUNNEL)
        {
        printf("TEST\n");
            // OK
            pthread_cond_signal(&train->condition);
        }
        else if(train->position == POS_LIGNE)
        {
        printf("TEST\n");
            // OK
            pthread_cond_signal(&train->condition);
        }
        else //if(train->position > POS_LIGNE)
        {
            printf("FINI\n");
        }
    }
    else // train->direction == ouest
    {
        if(train->position < POS_GARE)
        {
            printf("FINI\n");
        }
        else if(train->position == POS_GARE)
        {
        printf("TEST\n");
            // OK
            pthread_cond_signal(&train->condition);
        }
        else if(train->position == POS_GARAGE)
        {
        printf("TEST\n");
            // OK
            pthread_cond_signal(&train->condition);
            // Réserver gare
            if(train->type == M)
                utiliserVoie(train, &aiguillage.gare.voieB);
            else
                utiliserVoie(train, &aiguillage.gare.voieD);
        }
        else if(train->position == POS_TUNNEL)
        {
        printf("TEST\n");
            // OK
            pthread_cond_signal(&train->condition);
        }
        else if(train->position == POS_LIGNE)
        {
        printf("TEST\n");
            // OK
            pthread_cond_signal(&train->condition);
        }
        else //if(train->position > POS_LIGNE)
        {
            // Réserver ligne + tunnel + garage
            utiliserVoie(train, &aiguillage.ligne.voie);
            utiliserVoie(train, &aiguillage.tunnel.voie);
            if(train->type == M)
                utiliserVoie(train, &aiguillage.garage.voieMO);
            else if(train->type == GL)
                utiliserVoie(train, &aiguillage.garage.voieGL);
            else
                utiliserVoie(train, &aiguillage.garage.voieTGV);
        }
    }
}

/****************************************************************/
/********************** LIBERE RESSOURCES ***********************/
/****************************************************************/

/*
 * Libère tous les accès lorsque le train quitte
 * un morceau de l'aiguillage
 */
void libererAcces(Train_t *train)
{
    int prev;
    pthread_mutex_lock(&mutex);
    prev = train->position - suivant(train);
    if(prev == POS_GARE)
        libererAccesGare(train);
    else if(prev == POS_GARAGE)
        libererAccesGarage(train);
    else if(prev == POS_TUNNEL)
        libererAccesTunnel(train);
    else if(prev == POS_LIGNE)
        libererAccesLigne(train);
    pthread_mutex_unlock(&mutex);
}

void libererAccesGare(Train_t *train)
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

void libererAccesGarage(Train_t *train)
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

void libererAccesTunnel(Train_t *train)
{
    libererVoie(train, &aiguillage.tunnel.voie);
    if(aiguillage.tunnel.voie.nbAct == 0)
    {

    }
}

void libererAccesLigne(Train_t *train)
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
    printf("Garage_t : %d\t%d\t%d\t%d\n", aiguillage.garage.voieGL.nbAct, aiguillage.garage.voieME.nbAct, aiguillage.garage.voieMO.nbAct, aiguillage.garage.voieTGV.nbAct);
    printf("Tunnel_t  : %d\n", aiguillage.tunnel.voie.nbAct);
    printf("Ligne_t   : %d\n\n", aiguillage.ligne.voie.nbAct);
}
