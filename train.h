#ifndef TRAIN_H_INCLUDED
#define TRAIN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "structures.h"

#define TEMPS 1000000

typedef struct {
    pthread_t id;
    Type type;
    int position;
    Direction direction;
} Train;

void avancer(Train* train);

int suivant(Train *train);

int fini(Train *train);

Train initTrain(int i);

void printTrain(Train *train);

#endif // TRAIN_H_INCLUDED
