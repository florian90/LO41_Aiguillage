#ifndef AIGUILLAGE_H_INCLUDED
#define AIGUILLAGE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "train.h"
#include "structures.h"
#include "voie.h"
#include "list.h"

#define NBR_STOCK 10

typedef struct {
    Voie_t voieA;
    Voie_t voieB;
    Voie_t voieC;
    Voie_t voieD;
}Gare_t;

typedef struct {
    Voie_t voieTGV;
    Voie_t voieMO;
    Voie_t voieME;
    Voie_t voieGL;
}Garage_t;

typedef struct {
    Voie_t voie;
}Tunnel_t;

typedef struct {
    Voie_t voie;
}Ligne_t;

typedef struct {
    Gare_t gare;
    Garage_t garage;
    Tunnel_t tunnel;
    Ligne_t ligne;
    List_t listTGV;
    List_t listGL;
    List_t listM;
    int update;
}Aiguillage_t;

Gare_t initGare();
Garage_t initGarage();
Tunnel_t initTunnel();
Ligne_t initLigne();

void *initAiguillage(void *p);

int avance(Train_t *train);

// void acceder(Train_t* train);

void demandeAcces(Train_t *train);
void addToWaitList(Train_t *train);
void accederGare(Train_t* train);
void accederGarage(Train_t* train);
void accederTunnel(Train_t* train);
void accederLigne(Train_t* train);

void signalTrains();
int signalTrainList(List_t *list);
void signalTrain(Train_t *train);

void libererAcces(Train_t *train);
void libererAccesGare(Train_t *train);
void libererAccesGarage(Train_t *train);
void libererAccesTunnel(Train_t *train);
void libererAccesLigne(Train_t *train);

void printAiguillage();

#endif // AIGUILLAGE_H_INCLUDED
