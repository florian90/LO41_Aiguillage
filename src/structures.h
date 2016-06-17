#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#include <stdio.h>
#include <pthread.h>

// Nombre de trains
#define NB_THREAD 20

// Nombre de trains possibles dans les voies
#define NBR_STOCK 2

// Temps nécessaire pour traverser une voie
#define TEMPS 1000000

// Direction du train
typedef int Direction;

#define EST     (1<<0)
#define OUEST   (1<<1)
#define BIDIRR  (OUEST | EST)

// Type de train
typedef int Type;

#define TGV (1<<0)
#define GL  (1<<1)
#define M   (1<<2)
#define VAL_TOT (TGV | GL | M)

// Position de chaque partie de l'aiguillage
#define POS_OUEST   (0)
#define POS_GARE    (POS_OUEST+1)
#define POS_GARAGE  (POS_GARE+1)
#define POS_TUNNEL  (POS_GARAGE+1)
#define POS_LIGNE   (POS_TUNNEL+1)
#define POS_EST     (POS_LIGNE+1)

int getPriorite(Type t);

void printPos(int pos);

void dort(int i);

#endif // STRUCTURES_H_INCLUDED
