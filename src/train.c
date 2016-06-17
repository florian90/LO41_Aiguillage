#include "train.h"

Train_t initTrain(int i, Type t, Direction d)
{
	Train_t train;
	train.id = i;
	train.type = t;
	train.direction = d;
	train.position = (train.direction==EST?POS_OUEST:POS_EST);
	train.dort = false;
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
	if(train->dort)
		printf("\033[31m");
	else
		printf("\033[32m");
	if(train->direction == EST)
	{
		printf("[˳");
		switch(train->type)
		{
			case TGV : printf("TGV");break;
			case GL  : printf(" GL");break;
			case M   : printf(" M ");break;
		}
		printf("˳]-[˳%03d˳\\>", train->id);
	}

	if(train->direction == OUEST)
	{
		printf("</˳%03d˳]-[˳", train->id);
		switch(train->type)
		{
			case TGV : printf("TGV");break;
			case GL  : printf(" GL");break;
			case M   : printf(" M ");break;
		}
		printf("˳]");
	}
	printf("\033[0m\n");
}
