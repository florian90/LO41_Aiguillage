#include "train.h"


// </˳˳_˳˳]-[˳˳_˳˳]-[˳˳_˳˳]

// [˳˳_˳˳]-[˳˳_˳˳]-[˳˳_˳˳\>

Train_t initTrain(int i, Type t, Direction d)
{
    Train_t train;
    train.id = i;
    train.type = t;
    train.direction = d;
    train.position = (train.direction==EST?POS_OUEST:POS_EST);
    train.voie = NULL;
    return train;
}

Train_t initRandTrain(int i)
{
    Type type = 1<<(rand()%3);
    Direction d = rand()%2 + 1;
    return initTrain(i, type, d);
}

int suivant(Train_t *train)
{
    if(train->direction == EST)
        return +1;
    else
        return -1;
}

bool arrive(Train_t *train)
{
    if(train->direction == EST)
        return (train->position >= POS_EST);
    else
        return (train->position <= POS_OUEST);
}

void printTrain(Train_t *train)
{
    //printf("\033[%d;%dH", 15 + (train->id%2), (int) train->id*25);
    printf("Train %3d : ", (int) train->id);
    switch(train->direction)
    {
        case EST   : printf("EST   ");break;
        case OUEST : printf("OUEST ");break;
    }
    switch(train->type)
    {
        case TGV : printf("TGV ");break;
        case GL  : printf("GL  ");break;
        case M   : printf("M   ");break;
    }
    switch(train->position)
    {
        case 0: printf("Pos O  ");break;
        case 1: printf("Garre  ");break;
        case 2: printf("Garage ");break;
        case 3: printf("Tunnel ");break;
        case 4: printf("Ligne  ");break;
        case 5: printf("Pos E  ");break;
    }
    printf("\n");
    fflush(stdout);
}
