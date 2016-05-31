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
    if(train->direction == EST)
        return (train->position == POS_LIGNE + suivant(train));
    else
        return (train->position == POS_GARE + suivant(train));
}

Train initTrain(int i)
{
    Train train;
    train.id = i;
    train.direction = rand()%2 + 1;
    train.position = (train.direction-1)*5;
    train.type = 1<<(rand()%3);
    return train;
}

void printTrain(Train *train)
{
    printf("Train %d\t", (int) train->id);
    if(train->direction == EST)
        printf("EST\t");
    else
        printf("OUEST\t");

    if(train->type == TGV)
        printf("TGV\t");
    else if(train->type == GL)
        printf("GL\t");
    else
        printf("M\t");
    switch(train->position)
    {
        case 0: printf("Pos O\n");break;
        case 1: printf("Garre\n");break;
        case 2: printf("Garrage\n");break;
        case 3: printf("Tunnel\n");break;
        case 4: printf("Ligne\n");break;
        case 5: printf("Pos E\n");break;
    }
    fflush(stdout);
}
