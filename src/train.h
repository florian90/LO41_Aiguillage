#ifndef TRAIN_H_INCLUDED
#define TRAIN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "structures.h"
#include "voie.h"

typedef struct {
	int id;
	Type type;
	Direction direction;
	int position;
	bool dort;
} Train_t;

Train_t initTrain(int i, Type t, Direction d);
Train_t initRandTrain(int i);

int suivant(Train_t *train);

void avancer(Train_t* train);

bool arrive(Train_t *train);

void printTrain(Train_t *train);

#endif // TRAIN_H_INCLUDED
