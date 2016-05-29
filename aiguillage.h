#ifndef AIGUILLAGE_H_INCLUDED
#define AIGUILLAGE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "train.h"
#include "structures.h"

#define NBR_STOCK 10

typedef struct{
    int sens;
    int acces;
    int nbact;
    int nbmax;
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
}Garrage;

typedef struct {
    Voie voie;
}Tunnel;

typedef struct {
    Voie voie;
}Ligne;

typedef struct {
    Gare gare;
    Garrage garrage;
    Tunnel tunnel;
    Ligne ligne;
}Aiguillage;

Voie initVoie(int sens, int acces);
Voie initStock(int sens, int acces, int capacite);

Gare initGare();
Garrage initGarrage();
Tunnel initTunnel();
Ligne initLigne();

void initAiguillage();

void demandeAcces(Train* train);

void demandeAccesGare(Train* train);
void demandeAccesGarrage(Train* train);
void demandeAccesTunnel(Train* train);
void demandeAccesLigne(Train* train);

void libererAcces(Train *train);

void libererAccesGare(Train *train);
void libererAccesGarrage(Train *train);
void libererAccesTunnel(Train *train);
void libererAccesLigne(Train *train);

#endif // AIGUILLAGE_H_INCLUDED
