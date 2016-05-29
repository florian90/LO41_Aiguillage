#include "train.h"

int suivant(Train *train)
{
    if(train->direction == EST)
        return +1;
    else
        return -1;
}

int fini(Train *train)
{
    return (train->direction == EST && train->position > POS_LIGNE) || (train->direction == OUEST && train->position  < POS_GARE);
}

Train initTrain()
{
    Train train;
    train.id = pthread_self();
    srand(train.id);
    train.direction = srand()%2 + 1;
    train.position = (srand()%2) * 5;
    train.type = 1<<(srand()%3);
}
