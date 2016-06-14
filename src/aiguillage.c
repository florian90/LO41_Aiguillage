
#include "aiguillage.h"

Aiguillage_t aiguillage;

/****************************************************************/
/************************* INITALISATION ************************/
/****************************************************************/

#define NBR_TRAINS_INIT 10
Train_t trainsInit[10] = {
	{0, TGV	, OUEST	},
	{1, M	, EST	},
	{2, GL	, OUEST },
	{3, TGV	, OUEST },
	{4, M	, OUEST },
	{5, TGV	, EST },
	{6, GL	, OUEST },
	{7, TGV	, OUEST },
	{8, M	, EST },
	{9, TGV	, EST },
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

void initOuest()
{
	initArret(&aiguillage.ouest.voieEst, EST, VAL_TOT, NBR_STOCK, libererOuest);
	initVoie(&aiguillage.ouest.voieOuest, OUEST, VAL_TOT);
}

void initGare()
{
	initArret(&aiguillage.gare.voieA, EST ,M , NBR_STOCK, libererGareA);
	initCapa(&aiguillage.gare.voieB, OUEST, M, NBR_STOCK);
	initArret(&aiguillage.gare.voieC, EST, TGV | GL, NBR_STOCK, libererGareC);
	initCapa(&aiguillage.gare.voieD, OUEST, TGV | GL, NBR_STOCK);
}

void initGarage()
{
	initArret(&aiguillage.garage.voieTGV, BIDIRR, TGV, NBR_STOCK, libererGarageTGV);
	initArret(&aiguillage.garage.voieMO, OUEST, M, NBR_STOCK, libererGarageMO);
	initArret(&aiguillage.garage.voieME, EST, M, NBR_STOCK, libererGarageME);;
	initArret(&aiguillage.garage.voieGL, BIDIRR, GL, NBR_STOCK, libererGarageGL);
}

void initTunnel()
{
	initCapa(&aiguillage.tunnel.voie, BIDIRR, VAL_TOT, 1);
}

void initLigne()
{
	initVoie(&aiguillage.ligne.voie, BIDIRR, VAL_TOT);
}

void initEst()
{
	initVoie(&aiguillage.est.voieEst, EST, VAL_TOT);
	initArret(&aiguillage.est.voieOuest, OUEST, VAL_TOT, NBR_STOCK, libererEst);
}

/*
* Initialise toutes les parties de l'aiguillage
*/
void *initAiguillage(void *p)
{
	initOuest();
	initGare();
	initGarage();
	initTunnel();
	initLigne();
	initEst();
	aiguillage.update = 0;

	// Signal que l'aiguillage est prêt à recevoir les trains
	pthread_cond_signal(&condAiguillagePret);

	Type t;
	t = TGV;
	while(1)
	{
		//dort(500000);
		liberePriorite(t);
		if(t == TGV)
			t = GL;
		else if(t == GL)
			t = M;
		else
			t = TGV;
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
int liberePriorite(Type t)
{
	int i;
	for(i=0;i<NB_VOIE_STOP;i++)
	{
		(voieArret[i]->funcLiberer)(t);
	}
	return 0;
}

// Libération générale :
// Bloque la voie
// if(trains de ce type en attente)
// {
// 	pour chaque type de train et chaque sens :
// 	{
// 		Bloque les mutex sur les voies à  tester
// 		if( toutes les conditions sont vérifiées)
// 		{
// 			Utilise toutes les voies nécessaires
// 			Retire le train en attente
// 			libère le train
// 		}
// 		Débloque les mutex
// 	}
// }
// débloque la voie

int libererOuest(Type t)
{
	int res = 0;
	Voie_t *voie = &aiguillage.ouest.voieEst;
	// Bloque le mlutex de la voie actuelle ??
	pthread_mutex_lock(&voie->mutex);
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		// Séparer les cas en fonction du sens et du type du train
		if(t == M)
		{
			pthread_mutex_lock(&aiguillage.gare.voieA.mutex);
			if(peutAjouterTrain(&aiguillage.gare.voieA, EST)
			&& !supEnAttenteOppose(&aiguillage.gare.voieA, t, EST))
			{
				utiliserVoie(&aiguillage.gare.voieA, EST);
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
				res = 1;
			}
			pthread_mutex_unlock(&aiguillage.gare.voieA.mutex);
		}
		else
		{
			pthread_mutex_lock(&aiguillage.gare.voieC.mutex);
			if(peutAjouterTrain(&aiguillage.gare.voieC, EST)
			&& !supEnAttenteOppose(&aiguillage.gare.voieC, t, EST))
			{
				utiliserVoie(&aiguillage.gare.voieC, EST);
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
				res = 1;
			}
			pthread_mutex_unlock(&aiguillage.gare.voieC.mutex);
		}
	}
	pthread_mutex_unlock(&voie->mutex);
	return res;
}

int libererGareA(Type t)
{
	int res = 0;
	Voie_t *voie = &aiguillage.gare.voieA;
	// Si il y a des trains de ce type en attente sur la voie
	pthread_mutex_lock(&voie->mutex);
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		// Bloque le mutex de toutes les voies à réserver
		pthread_mutex_lock(&aiguillage.garage.voieME.mutex);
		if(peutAjouterTrain(&aiguillage.garage.voieME, EST)
		&& !supEnAttenteOppose(&aiguillage.garage.voieME, t, EST))
		{
			// Réserve toutes les voies nécessaires
			utiliserVoie(&aiguillage.garage.voieME, EST);

			// Libère la condition sur la voie actuelle
			retirerAttente(voie, t);
			pthread_cond_signal(&voie->condition[numTabType(t)]);
			res = 1;
		}
		// Libère tous les mutex bloqués
		pthread_mutex_unlock(&aiguillage.garage.voieME.mutex);
	}
	pthread_mutex_unlock(&voie->mutex);
	return res;
}

int libererGareC(Type t)
{
	int res = 0;
	Voie_t *voie = &aiguillage.gare.voieC;
	pthread_mutex_lock(&voie->mutex);
	// Si il y a des trains de ce type en attente sur la voie
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		if(t == TGV)
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.garage.voieTGV.mutex);
			if(peutAjouterTrain(&aiguillage.garage.voieTGV, EST)
			&& !supEnAttenteOppose(&aiguillage.garage.voieTGV, t, EST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.garage.voieTGV, EST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
				res = 1;
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.garage.voieTGV.mutex);
		}
		else // GL
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.garage.voieGL.mutex);
			if(peutAjouterTrain(&aiguillage.garage.voieGL, EST)
			&& !supEnAttenteOppose(&aiguillage.garage.voieGL, t, EST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.garage.voieGL, EST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
				res = 1;
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.garage.voieGL.mutex);
		}
	}
	pthread_mutex_unlock(&voie->mutex);
	return res;
}

int libererGarageTGV(Type t)
{
	int res = 0;
	Voie_t *voie = &aiguillage.garage.voieTGV;
	pthread_mutex_lock(&voie->mutex);
	// Si il y a des trains de ce type en attente sur la voie
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		if(voie->sensAct == EST)
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.tunnel.voie.mutex);
			pthread_mutex_lock(&aiguillage.ligne.voie.mutex);
			pthread_mutex_lock(&aiguillage.est.voieOuest.mutex);
			if(peutAjouterTrain(&aiguillage.tunnel.voie, EST)
			&& peutAjouterTrain(&aiguillage.ligne.voie, EST)
			&& peutAjouterTrain(&aiguillage.est.voieEst, EST)
			&& !supEnAttenteOppose(&aiguillage.est.voieEst, t, EST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.tunnel.voie, EST);
				utiliserVoie(&aiguillage.ligne.voie, EST);
				utiliserVoie(&aiguillage.est.voieOuest, EST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
				res = 1;
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.tunnel.voie.mutex);
			pthread_mutex_unlock(&aiguillage.ligne.voie.mutex);
			pthread_mutex_unlock(&aiguillage.est.voieOuest.mutex);
		}
		else // sens == OUEST
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.gare.voieD.mutex);
			if(peutAjouterTrain(&aiguillage.gare.voieD,  OUEST)
			&& !supEnAttenteOppose(&aiguillage.gare.voieD, t, OUEST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.gare.voieD, OUEST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
				res = 1;
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.gare.voieD.mutex);
		}
	}
	pthread_mutex_unlock(&voie->mutex);
	return res;
}

int libererGarageMO(Type t)
{
	int res = 0;
	Voie_t *voie = &aiguillage.garage.voieMO;
	pthread_mutex_lock(&voie->mutex);
	// Si il y a des trains de ce type en attente sur la voie
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		// Bloque le mutex de toutes les voies à réserver
		pthread_mutex_lock(&aiguillage.gare.voieB.mutex);
		if(peutAjouterTrain(&aiguillage.gare.voieB,  OUEST)
		&& !supEnAttenteOppose(&aiguillage.gare.voieB, t, OUEST))
		{
			// Réserve toutes les voies nécessaires
			utiliserVoie(&aiguillage.gare.voieB, OUEST);

			// Libère la condition sur la voie actuelle
			retirerAttente(voie, t);
			pthread_cond_signal(&voie->condition[numTabType(t)]);
			res = 1;
		}
		// Libère tous les mutex bloqués
		pthread_mutex_unlock(&aiguillage.gare.voieB.mutex);
	}
	pthread_mutex_unlock(&voie->mutex);
	return res;
}

int libererGarageME(Type t)
{
	int res = 0;
	Voie_t *voie = &aiguillage.garage.voieME;
	pthread_mutex_lock(&voie->mutex);
	// Si il y a des trains de ce type en attente sur la voie
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		// Bloque le mutex de toutes les voies à réserver
		pthread_mutex_lock(&aiguillage.tunnel.voie.mutex);
		pthread_mutex_lock(&aiguillage.ligne.voie.mutex);
		pthread_mutex_lock(&aiguillage.est.voieOuest.mutex);
		if(peutAjouterTrain(&aiguillage.tunnel.voie, EST)
		&& peutAjouterTrain(&aiguillage.ligne.voie, EST)
		&& peutAjouterTrain(&aiguillage.est.voieEst, EST)
		&& !supEnAttenteOppose(&aiguillage.est.voieEst, t, EST))
		{
			// Réserve toutes les voies nécessaires
			utiliserVoie(&aiguillage.tunnel.voie, EST);
			utiliserVoie(&aiguillage.ligne.voie, EST);
			utiliserVoie(&aiguillage.est.voieOuest, EST);

			// Libère la condition sur la voie actuelle
			retirerAttente(voie, t);
			pthread_cond_signal(&voie->condition[numTabType(t)]);
			res = 1;
		}
		// Libère tous les mutex bloqués
		pthread_mutex_unlock(&aiguillage.est.voieOuest.mutex);
		pthread_mutex_unlock(&aiguillage.ligne.voie.mutex);
		pthread_mutex_unlock(&aiguillage.tunnel.voie.mutex);
	}
	pthread_mutex_unlock(&voie->mutex);
	return res;
}

int libererGarageGL(Type t)
{
	int res = 0;
	Voie_t *voie = &aiguillage.garage.voieGL;
	pthread_mutex_lock(&voie->mutex);
	// Si il y a des trains de ce type en attente sur la voie
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		if(voie->sensAct == EST)
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.tunnel.voie.mutex);
			pthread_mutex_lock(&aiguillage.ligne.voie.mutex);
			pthread_mutex_lock(&aiguillage.est.voieOuest.mutex);
			if(peutAjouterTrain(&aiguillage.tunnel.voie, EST)
			&& peutAjouterTrain(&aiguillage.ligne.voie, EST)
			&& peutAjouterTrain(&aiguillage.est.voieEst, EST)
			&& !supEnAttenteOppose(&aiguillage.est.voieEst, t, EST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.tunnel.voie, EST);
				utiliserVoie(&aiguillage.ligne.voie, EST);
				utiliserVoie(&aiguillage.est.voieOuest, EST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
				res = 1;
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.est.voieOuest.mutex);
			pthread_mutex_unlock(&aiguillage.ligne.voie.mutex);
			pthread_mutex_unlock(&aiguillage.tunnel.voie.mutex);
		}
		else // sens == OUEST
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.gare.voieD.mutex);
			if(peutAjouterTrain(&aiguillage.gare.voieD,  OUEST)
			&& !supEnAttenteOppose(&aiguillage.gare.voieD, t, OUEST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.gare.voieD, OUEST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
				res = 1;
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.gare.voieD.mutex);
		}
	}
	pthread_mutex_unlock(&voie->mutex);
	return res;
}

int libererEst(Type t)
{
	int res = 0;
	Voie_t *voie = &aiguillage.est.voieOuest;
	// Bloque le mlutex de la voie actuelle ??
	pthread_mutex_lock(&voie->mutex);
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		Voie_t *voieGarage;
		if(t == TGV)
		{
			voieGarage = &aiguillage.garage.voieTGV;
		}
		else if(t == GL)
		{
			voieGarage = &aiguillage.garage.voieGL;
		}
		else
		{
			voieGarage = &aiguillage.garage.voieMO;
		}

		pthread_mutex_lock(&aiguillage.ligne.voie.mutex);
		pthread_mutex_lock(&aiguillage.tunnel.voie.mutex);
		pthread_mutex_lock(&voieGarage->mutex);
		if(peutAjouterTrain(&aiguillage.ligne.voie, OUEST)
		&& peutAjouterTrain(&aiguillage.tunnel.voie, OUEST)
		&& peutAjouterTrain(voieGarage, OUEST)
		&& !supEnAttenteOppose(voieGarage, t, OUEST))
		{
			utiliserVoie(&aiguillage.ligne.voie, OUEST);
			utiliserVoie(&aiguillage.tunnel.voie, OUEST);
			utiliserVoie(voieGarage, OUEST);
			retirerAttente(voie, t);
			pthread_cond_signal(&voie->condition[numTabType(t)]);
			res = 1;
		}
		pthread_mutex_unlock(&aiguillage.ligne.voie.mutex);
		pthread_mutex_unlock(&aiguillage.tunnel.voie.mutex);
		pthread_mutex_unlock(&voieGarage->mutex);

	}
	pthread_mutex_unlock(&voie->mutex);
	return res;
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
	printAiguillage();
	if(canStop(act))
	{
		pthread_mutex_lock(&act->mutex);
		train->dort = true;
		// attends libération du suivant
		ajouterAttente(act, train->type);
		pthread_cond_wait(&act->condition[numTabType(train->type)], &act->mutex);
		train->dort = false;
		aiguillage.update = 1;
		pthread_mutex_unlock(&act->mutex);
	}
	// printAiguillage();

	if(!peutUtiliser(next, train->direction))
	{
		// INTERBLOCAGE
		pthread_mutex_lock(&mutexEcriture);
		printf("INTERBLOCAGE : de %d en %d\n", train->id, train->position);
		pthread_mutex_unlock(&mutexEcriture);
		return -1;
	}
	dort(TEMPS);

	// Libère les ressources
	pthread_mutex_lock(&act->mutex);
	train->position += suivant(train);
	aiguillage.update = 1;
	libererVoie(act);
	pthread_mutex_unlock(&act->mutex);

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
