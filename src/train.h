#ifndef TRAIN_H_INCLUDED
#define TRAIN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "structures.h"

#define TEMPS 100

typedef struct {
    int id;
    Type_t type;
    Direction_t direction;
    int position;
    pthread_cond_t condition;
    // pthread_cont_t condition -> si attente : mettre dans la file + attente
} Train_t;

Train_t initTrain(int i, Type_t t, Direction_t d);
Train_t initRandTrain(int i);

int suivant(Train_t *train);

void avancer(Train_t* train);

int arrive(Train_t *train);

void printTrain(Train_t *train);

#endif // TRAIN_H_INCLUDED
