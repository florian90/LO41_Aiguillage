#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#include <stdio.h>
#include <pthread.h>

#define ESPACE 20

#define NB_THREAD 2

#define NBR_STOCK -1

#define TEMPS 500000

#define EST     (1<<0)
#define OUEST   (1<<1)
#define BIDIRR  (OUEST | EST)

#define TGV (1<<0)
#define GL  (1<<1)
#define M   (1<<2)
#define VAL_TOT (TGV | GL | M)

#define POS_OUEST   (0)
#define POS_GARE    (POS_OUEST+1)
#define POS_GARAGE  (POS_GARE+1)
#define POS_TUNNEL  (POS_GARAGE+1)
#define POS_LIGNE   (POS_TUNNEL+1)
#define POS_EST     (POS_LIGNE+1)

typedef int Type;

typedef int Direction;

int getPriorite(Type t);

void safePuts(char * str);

void printPos(int pos);

#endif // STRUCTURES_H_INCLUDED
