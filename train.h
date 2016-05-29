#ifndef TRAIN_H_INCLUDED
#define TRAIN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "structures.h"

#define TEMPS 100000

typedef struct {
    int id;
    Type type;
    int position;
    Direction direction;
} Train;

void avancer(Train* train);

int suivant(Train *train);

int fini(Train *train);

Train initTrain();

#endif // TRAIN_H_INCLUDED
