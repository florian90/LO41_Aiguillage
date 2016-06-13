
#include "aiguillage.h"

Aiguillage_t aiguillage;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/****************************************************************/
/************************* INITALISATION ************************/
/****************************************************************/

#define NBR_TRAINS_INIT 10
Train_t trainsInit[10] = {  {0, TGV	, EST	},
{1, TGV	, OUEST	},
{2, GL	, OUEST },
{3, M	, EST   },
{4, GL	, OUEST },
{5, TGV	, EST   },
{6, GL	, OUEST },
{7, M	, EST   },
{8, TGV	, OUEST },
{9, M	, EST   },
};

Train_t initTrainAiguillage(int no)
{
	Train_t train;
	if(no < NBR_TRAINS_INIT)
	{
		train = trainsInit[no];
	}
	else
	{
		train = initRandTrain(no);
	}

	train.id = no;
	if(train.direction == EST)
	{
		train.position = POS_OUEST;
	}
	else
	{
		train.position = POS_EST;
	}
	utiliserVoie(actVoie(&train), train.position);
	return train;
}

Sortie_t initOuest()
{
	Sortie_t o;
	o.voieEst = initVoie(EST, VAL_TOT, true);
	o.voieOuest = initVoie(OUEST, VAL_TOT, false);
	return o;
}

Gare_t initGare()
{
	Gare_t g;
	g.voieA = initCapa(EST ,VAL_M, NBR_STOCK, true);
	g.voieB = initCapa(OUEST, VAL_M, NBR_STOCK, false);
	g.voieC = initCapa(EST, VAL_TGV | VAL_GL, NBR_STOCK, true);
	g.voieD = initCapa(OUEST, VAL_TGV | VAL_GL, NBR_STOCK, false);
	return g;
}

Garage_t initGarage()
{
	Garage_t g;
	g.voieTGV = initCapa(BIDIRR, VAL_TGV, NBR_STOCK, true);
	g.voieMO  = initCapa(OUEST, VAL_M, NBR_STOCK, true);
	g.voieME  = initCapa(EST, VAL_M, NBR_STOCK, true);;
	g.voieGL  = initCapa(BIDIRR, VAL_GL, NBR_STOCK, true);
	return g;
}

Tunnel_t initTunnel()
{
	Tunnel_t t;
	t.voie = initCapa(BIDIRR, VAL_TOT, 1, false);
	return t;
}

Ligne_t initLigne()
{
	Ligne_t l;
	l.voie = initVoie(BIDIRR, VAL_TOT, false);
	return l;
}

Sortie_t initEst()
{
	Sortie_t e;
	e.voieEst = initVoie(EST, VAL_TOT, false);
	e.voieOuest = initVoie(OUEST, VAL_TOT, true);
	return e;
}

/*
* Initialise toutes les parties de l'aiguillage
*/
void *initAiguillage(void *p)
{
	int priorite, res;
	aiguillage.ouest = initOuest();
	aiguillage.gare = initGare();
	aiguillage.garage = initGarage();
	aiguillage.tunnel = initTunnel();
	aiguillage.ligne = initLigne();
	aiguillage.est = initEst();
	aiguillage.update = 0;

	// Signal que l'aiguillage est prêt à recevoir les trains
	pthread_cond_signal(&condAiguillagePret);

	priorite = 1;
	while(1)
	{
		//dort(500000);
		res = liberePriorite(priorite);
		if(res == -1)
		{
			//safePuts("RESTART\n");
			aiguillage.update = 0;
			priorite = 1;
			printAiguillage();
		}
		else if(res != 0 && ++priorite > 3)
			priorite = 1;
	}
	return NULL;
}

#define NB_VOIE_STOP 8
Voie_t *voieArret[8] = {
	&aiguillage.ouest.voieEst,
	&aiguillage.gare.voieA,
	&aiguillage.gare.voieC,
	&aiguillage.garage.voieTGV,
	&aiguillage.garage.voieMO,
	&aiguillage.garage.voieME,
	&aiguillage.garage.voieGL,
	&aiguillage.est.voieOuest
};

/*
* Libère toutes les voies qu'il peut et qui attendent avec une priorité donnée
*/
int liberePriorite(int priorite)
{
	int i, res;
	Voie_t *voie;
	for(i=0;i<NB_VOIE_STOP;i++)
	{
		voie = voieArret[i];
		res = (voie->funcLiberer)(int priorite);
		if(res != 0)
			return res;
	}
}

int libererOuest(Type t)
{
	Voie_t *voie = &aiguillage.ouest.voieEst;
	pthread_mutex_lock(&voie->mutex);
	if(voie.attends)
	{
		;
	}
	pthread_mutex_unlock(&voie->mutex);
	return 0;
}

int libererGareA(Type t)
int libererGareB(Type t)
int libererGarageTGV(Type t)
int libererGarageMO(Type t)
int libererGarageME(Type t)
int libererGarageGL(Type t)
int libererEst(Type t)

int funcihbjn(int priorite)
{
	Voie_t *voie;

	if(aiguillage.update)
	return -1;

	// EST
	voie = &aiguillage.ouest.voieEst;
	if(voie->priorite <= priorite)
	{
		libere(voie);
	}

	if(aiguillage.update)
	return -1;

	// Gare
	voie = &aiguillage.gare.voieA;
	if(voie->priorite <= priorite)
	{
		// if(voieME.nbAct < voieME.nbMax) ????
		libere(voie);
	}

	if(aiguillage.update)
	return -1;

	voie = &aiguillage.gare.voieC;
	if(voie->priorite <= priorite)
	{
		if(peutUtiliser(&aiguillage.tunnel.voie, EST) && peutUtiliser(&aiguillage.ligne.voie, EST))
		{ // Si on peut continuer après le tunnel
			if(voie->priorite == 1)
			{// TGV || GL
				if(peutUtiliser(&aiguillage.garage.voieTGV, EST) && peutUtiliser(&aiguillage.garage.voieGL, EST))
				{
					safePuts("TEST 1\n");
					libere(voie);
				}
			}
			else if(voie->priorite == 2)
			{ // GL uniquement
				if(peutUtiliser(&aiguillage.garage.voieGL, EST))
				{
					safePuts("TEST 2\n");
					libere(voie);
				}
			}
		}
	}

	if(aiguillage.update)
	return -1;

	// Garage
	int i;
	voie = &aiguillage.garage.voieTGV;
	for(i=0;i<2;i++)
	{
		if(voie->priorite <= priorite)
		{
			if(voie->sensAct == EST)
			{
				if(peutUtiliser(&aiguillage.tunnel.voie, EST) && peutUtiliser(&aiguillage.ligne.voie, EST))
				{ // Si on peut continuer après le tunnel
					libere(voie);
				}
			}
			else // OUEST
			{
				libere(voie);
			}
		}
		voie = &aiguillage.garage.voieGL;
	}

	if(aiguillage.update)
		return -1;

	voie = &aiguillage.garage.voieMO;
	if(voie->priorite <= priorite)
	{
		libere(voie);
	}

	if(aiguillage.update)
		return -1;

	voie = &aiguillage.garage.voieME;
	if(voie->priorite <= priorite)
	{
		if(peutUtiliser(&aiguillage.ligne.voie, EST) && peutUtiliser(&aiguillage.tunnel.voie, EST))
		{
			libere(voie);
		}
	}

	if(aiguillage.update)
		return -1;

	// EST
	voie = &aiguillage.est.voieOuest;
	if(voie->priorite <= priorite)
	{
		if(peutUtiliser(&aiguillage.ligne.voie, OUEST) && peutUtiliser(&aiguillage.tunnel.voie, OUEST))
		{
			libere(voie);
		}
	}

	if(aiguillage.update)
		return -1;
	return 0;
}

void libere(Voie_t *voie, Type t)
{
	pthread_cond_signal(&voie->condition);
	finiAttente(voie, t);
}

void libereOuest(int priorite)
{
	Voie_t *voie = &aiguillage.ouest.voieEst;
	pthread_mutex_lock(&mutex);
	if(voie->priorite <= priorite)
	{
		if(priorite)
		{
			libere(voie);
		}
	}
	pthread_mutex_unlock(&mutex);
}

void libereEst(int priorite)
{
	Voie_t *voie = &aiguillage.est.voieOuest;
	pthread_mutex_lock(&mutex);
	if(voie->priorite <= priorite)
	{
		if(peutUtiliser(&aiguillage.ligne.voie, OUEST) && peutUtiliser(&aiguillage.tunnel.voie, OUEST))
		{
			libere(voie);
		}
	}
	pthread_mutex_unlock(&mutex);
}

/****************************************************************/
/*************************** GET VOIE ***************************/
/****************************************************************/

Voie_t *getVoie(int position, Type type, Direction direction)
{
	switch (position) {
		case POS_OUEST:
		{
			if(direction == EST)
			return &aiguillage.ouest.voieEst;
			else
			return &aiguillage.ouest.voieOuest;
		}
		case POS_GARE:
		{
			if(type == M)
			{
				if(direction == EST)
				return &aiguillage.gare.voieA;
				else // OUEST
				return &aiguillage.gare.voieB;
			}
			else // GL | TGV
			{
				if(direction == EST)
				return &aiguillage.gare.voieC;
				else // OUEST
				return &aiguillage.gare.voieD;
			}
		}
		case POS_GARAGE:
		{
			if(type == TGV)
			return &aiguillage.garage.voieTGV;
			else if(type == GL)
			return &aiguillage.garage.voieGL;
			else // M
			{
				if(direction == EST)
				return &aiguillage.garage.voieME;
				else // OUEST
				return &aiguillage.garage.voieMO;
			}
		}
		case POS_TUNNEL:
		{
			return &aiguillage.tunnel.voie;
		}
		case POS_LIGNE:
		{
			return &aiguillage.ligne.voie;
		}
		case POS_EST:
		{
			if(direction == EST)
			return &aiguillage.est.voieEst;
			else
			return &aiguillage.est.voieOuest;
		}
	}
	return NULL;
}

Voie_t *actVoie(Train_t *train)
{
	return getVoie(train->position, train->type, train->direction);
}

Voie_t *nextVoie(Train_t *train)
{
	return getVoie(train->position + suivant(train), train->type, train->direction);
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

int avance(Train_t *train)
{
	// Si peut s'arréter
	//     S'arrète + attends de pouvoir partir
	// Réserve l'accès au suivant
	// Si il ne peut pas avancer :
	//     retourne -1 => INTERBLOCAGE
	// avancer
	// Libère la voie précédente

	Voie_t *act;
	Voie_t *next;
	act = actVoie(train);
	next = nextVoie(train);
	if(canStop(act))
	{
		train->dort = true;
		setPrio(act, train->type);
		// attends libération du suivant
		pthread_cond_wait(&act->condition, &mutex);
		train->dort = false;
	}

	aiguillage.update = 1;
	if(!utiliserVoie(next, train->direction))
	{
		// INTERBLOCAGE
		pthread_mutex_lock(&mutexEcriture);
		printf("INTERBLOCAGE : de %d en %d\n", train->id, train->position);
		pthread_mutex_unlock(&mutexEcriture);
		return -1;
	}
	dort(TEMPS);
	train->position += suivant(train);
	aiguillage.update = 1;
	libererVoie(act);

	// printAiguillage();

	return 0;
}

/*
* Affiche les détails de l'aiguillage
*/
void printAiguillage()
{
	pthread_mutex_lock(&mutexEcriture);
	fflush(stdout);
	// printf("\033[2J");
	// printf("\033[1;1H");
	int i, pos, nb=1, nbMax = 0, p0;
	p0 = 0;
	for(pos = p0; pos <= POS_EST; pos++)
	{
		nb = 1;
		// droite
		printf("\033[%dC", (pos-p0)*19);
		printPos(pos);
		for(i = 0;i<NB_THREAD; i++)
		{
			if(trains[i].position == pos)
			{
				// droite
				printf("\033[%dC", (pos-p0)*19);
				printTrain(&trains[i]);
				nb++;
			}
		}
		if(nb > nbMax)
		nbMax = nb;
		// haut
		printf("\033[%dA", nb);
	}
	// bas
	if(nbMax != 0)
	printf("\033[%dB", nbMax);
	printf("\n");
	fflush(stdout);
	pthread_mutex_unlock(&mutexEcriture);
}

void dort(int i)
{
	long j = 1000*(long) i;
	struct timespec t;
	t.tv_sec  = j/1000000000L;
	t.tv_nsec = j%1000000000L;
	nanosleep(&t, NULL);
}
