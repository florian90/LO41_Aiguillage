#ifndef AIGUILLAGE_H_INCLUDED
#define AIGUILLAGE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "train.h"
#include "structures.h"

#define NBR_STOCK 10

typedef struct{
    Direction sens;
    Direction sensAct;
    Type acces;
    int nbAct;
    int nbMax;
} Voie;

typedef struct {
    Voie voieA;
    Voie voieB;
    Voie voieC;
    Voie voieD;
}Gare;

typedef struct {
    Voie voieTGV;
    Voie voieMO;
    Voie voieME;
    Voie voieGL;
}Garage;

typedef struct {
    Voie voie;
}Tunnel;

typedef struct {
    Voie voie;
}Ligne;

typedef struct {
    Gare gare;
    Garage garage;
    Tunnel tunnel;
    Ligne ligne;
}Aiguillage;

Voie initVoie(int sens, int acces);
Voie initStock(int sens, int acces, int capacite);

Gare initGare();
Garage initGarage();
Tunnel initTunnel();
Ligne initLigne();

void initAiguillage();

void utiliserVoie(Train *train, Voie *voie);
void libererVoie(Train *train, Voie *voie);

int peutAcceder(Train *train, Voie *voie);

void acceder(Train* train);

void accederGare(Train* train);
void accederGarage(Train* train);
void accederTunnel(Train* train);
void accederLigne(Train* train);

void libererAcces(Train *train);

void libererAccesGare(Train *train);
void libererAccesGarage(Train *train);
void libererAccesTunnel(Train *train);
void libererAccesLigne(Train *train);

void printAiguillage();

#endif // AIGUILLAGE_H_INCLUDED
