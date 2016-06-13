
#include "structures.h"

extern pthread_mutex_t mutexEcriture;

int getPriorite(Type t)
{
	if(t == TGV)
		return 1;
	else if (t == GL)
		return 2;
	else
		return 3;
}

void safePuts(char *str)
{
	pthread_mutex_lock(&mutexEcriture);
	printf("%s", str);
	fflush(stdout);
	pthread_mutex_unlock(&mutexEcriture);
}

void printPos(int pos)
{
	 switch(pos)
	{
		case POS_OUEST:		printf("OUEST\n");	break;
		case POS_GARE: 		printf("GARE\n");	break;
		case POS_GARAGE: 	printf("GARAGE\n");	break;
		case POS_TUNNEL: 	printf("TUNNEL\n");	break;
		case POS_LIGNE: 	printf("LIGNE\n");	break;
		case POS_EST: 		printf("EST\n");	break;
	}
}