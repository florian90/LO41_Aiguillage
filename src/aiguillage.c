
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

	// Signal que l'aiguillage est prêt à recevoir les trains
	pthread_cond_signal(&condAiguillagePret);

	Type t;
	t = TGV;
	while(1)
	{
		int i;
		for(i=0;i<NB_VOIE_STOP;i++)
		{
			(voieArret[i]->funcLiberer)(t);
		}
		if(t == TGV)
			t = GL;
		else if(t == GL)
			t = M;
		else
			t = TGV;
	}
	return NULL;
}

void *affichage(void *p)
{
	while(1)
	{
		printAiguillage();
		dort(TEMPS);
	}
}

// Libération générale :
// Bloque la voie
// if(trains de ce type en attente et pas de trains plus prioritaire)
// {
// 	pour chaque type de train et chaque sens :
// 	{
// 		Bloque les mutex sur les voies à  tester
// 		if(toutes les conditions sont vérifiées)
// 		{
// 			Utilise toutes les ressources nécessaires
// 			Retire le train en attente
// 			libère le train
// 		}
// 		Débloque les mutex
// 	}
// }
// débloque la voie

void libererOuest(Type t)
{
	Voie_t *voie = &aiguillage.ouest.voieEst;
	// Bloque le mlutex de la voie actuelle ??
	pthread_mutex_lock(&voie->mutex);
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		// Séparer les cas en fonction du sens et du type du train
		if(t == M)
		{
			pthread_mutex_lock(&aiguillage.gare.voieA.mutex);
			if(peutAjouterTrain(&aiguillage.gare.voieA, EST))
			{
				utiliserVoie(&aiguillage.gare.voieA, EST);
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
			}
			pthread_mutex_unlock(&aiguillage.gare.voieA.mutex);
		}
		else
		{
			pthread_mutex_lock(&aiguillage.gare.voieC.mutex);
			if(peutAjouterTrain(&aiguillage.gare.voieC, EST))
			{
				utiliserVoie(&aiguillage.gare.voieC, EST);
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
			}
			pthread_mutex_unlock(&aiguillage.gare.voieC.mutex);
		}
	}
	pthread_mutex_unlock(&voie->mutex);
}

void libererGareA(Type t)
{
	Voie_t *voie = &aiguillage.gare.voieA;
	// Si il y a des trains de ce type en attente sur la voie
	pthread_mutex_lock(&voie->mutex);
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		// Bloque le mutex de toutes les voies à réserver
		pthread_mutex_lock(&aiguillage.garage.voieME.mutex);
		if(peutAjouterTrain(&aiguillage.garage.voieME, EST))
		{
			// Réserve toutes les voies nécessaires
			utiliserVoie(&aiguillage.garage.voieME, EST);

			// Libère la condition sur la voie actuelle
			retirerAttente(voie, t);
			pthread_cond_signal(&voie->condition[numTabType(t)]);
		}
		// Libère tous les mutex bloqués
		pthread_mutex_unlock(&aiguillage.garage.voieME.mutex);
	}
	pthread_mutex_unlock(&voie->mutex);
}

void libererGareC(Type t)
{
	Voie_t *voie = &aiguillage.gare.voieC;
	pthread_mutex_lock(&voie->mutex);
	// Si il y a des trains de ce type en attente sur la voie
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		if(t == TGV)
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.garage.voieTGV.mutex);
			if(peutAjouterTrain(&aiguillage.garage.voieTGV, EST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.garage.voieTGV, EST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.garage.voieTGV.mutex);
		}
		else // GL
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.garage.voieGL.mutex);
			if(peutAjouterTrain(&aiguillage.garage.voieGL, EST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.garage.voieGL, EST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.garage.voieGL.mutex);
		}
	}
	pthread_mutex_unlock(&voie->mutex);
}

void libererGarageTGV(Type t)
{
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
			pthread_mutex_lock(&aiguillage.est.voieEst.mutex);
			if(peutAjouterTrain(&aiguillage.tunnel.voie, EST)
			&& peutAjouterTrain(&aiguillage.ligne.voie, EST)
			&& peutAjouterTrain(&aiguillage.est.voieEst, EST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.tunnel.voie, EST);
				utiliserVoie(&aiguillage.ligne.voie, EST);
				utiliserVoie(&aiguillage.est.voieOuest, EST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.tunnel.voie.mutex);
			pthread_mutex_unlock(&aiguillage.ligne.voie.mutex);
			pthread_mutex_unlock(&aiguillage.est.voieEst.mutex);
		}
		else // sens == OUEST
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.gare.voieD.mutex);
			if(peutAjouterTrain(&aiguillage.gare.voieD,  OUEST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.gare.voieD, OUEST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.gare.voieD.mutex);
		}
	}
	pthread_mutex_unlock(&voie->mutex);
}

void libererGarageMO(Type t)
{
	Voie_t *voie = &aiguillage.garage.voieMO;
	pthread_mutex_lock(&voie->mutex);
	// Si il y a des trains de ce type en attente sur la voie
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		// Bloque le mutex de toutes les voies à réserver
		pthread_mutex_lock(&aiguillage.gare.voieB.mutex);
		if(peutAjouterTrain(&aiguillage.gare.voieB,  OUEST))
		{
			// Réserve toutes les voies nécessaires
			utiliserVoie(&aiguillage.gare.voieB, OUEST);

			// Libère la condition sur la voie actuelle
			retirerAttente(voie, t);
			pthread_cond_signal(&voie->condition[numTabType(t)]);
		}
		// Libère tous les mutex bloqués
		pthread_mutex_unlock(&aiguillage.gare.voieB.mutex);
	}
	pthread_mutex_unlock(&voie->mutex);
}

void libererGarageME(Type t)
{
	Voie_t *voie = &aiguillage.garage.voieME;
	pthread_mutex_lock(&voie->mutex);
	// Si il y a des trains de ce type en attente sur la voie
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		// Bloque le mutex de toutes les voies à réserver
		pthread_mutex_lock(&aiguillage.garage.voieTGV.mutex);
		pthread_mutex_lock(&aiguillage.garage.voieGL.mutex);
		pthread_mutex_lock(&aiguillage.tunnel.voie.mutex);
		pthread_mutex_lock(&aiguillage.ligne.voie.mutex);
		pthread_mutex_lock(&aiguillage.est.voieEst.mutex);
		pthread_mutex_lock(&aiguillage.est.voieOuest.mutex);
		if(peutAjouterTrain(&aiguillage.tunnel.voie, EST)
		&& peutAjouterTrain(&aiguillage.ligne.voie, EST)
		&& peutAjouterTrain(&aiguillage.est.voieEst, EST)
		&& estPrioritaireSur(t, &aiguillage.garage.voieTGV, EST)
		&& estPrioritaireSur(t, &aiguillage.garage.voieGL, EST)
		&& estPrioritaireSur(t, &aiguillage.est.voieOuest, OUEST))
		{
			// Réserve toutes les voies nécessaires
			utiliserVoie(&aiguillage.tunnel.voie, EST);
			utiliserVoie(&aiguillage.ligne.voie, EST);
			utiliserVoie(&aiguillage.est.voieOuest, EST);

			// Libère la condition sur la voie actuelle
			retirerAttente(voie, t);
			pthread_cond_signal(&voie->condition[numTabType(t)]);
		}
		// Libère tous les mutex bloqués
		pthread_mutex_unlock(&aiguillage.garage.voieTGV.mutex);
		pthread_mutex_unlock(&aiguillage.garage.voieGL.mutex);
		pthread_mutex_unlock(&aiguillage.tunnel.voie.mutex);
		pthread_mutex_unlock(&aiguillage.ligne.voie.mutex);
		pthread_mutex_unlock(&aiguillage.est.voieEst.mutex);
		pthread_mutex_unlock(&aiguillage.est.voieOuest.mutex);
	}
	pthread_mutex_unlock(&voie->mutex);
}

void libererGarageGL(Type t)
{
	Voie_t *voie = &aiguillage.garage.voieGL;
	pthread_mutex_lock(&voie->mutex);
	// Si il y a des trains de ce type en attente sur la voie
	if(enAttente(voie, t) && !supEnAttente(voie, t))
	{
		if(voie->sensAct == EST)
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.garage.voieTGV.mutex);
			pthread_mutex_lock(&aiguillage.tunnel.voie.mutex);
			pthread_mutex_lock(&aiguillage.ligne.voie.mutex);
			pthread_mutex_lock(&aiguillage.est.voieOuest.mutex);
			if(peutAjouterTrain(&aiguillage.tunnel.voie, EST)
			&& peutAjouterTrain(&aiguillage.ligne.voie, EST)
			&& estPrioritaireSur(t, &aiguillage.garage.voieTGV, EST)
			&& estPrioritaireSur(t, &aiguillage.est.voieOuest, OUEST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.tunnel.voie, EST);
				utiliserVoie(&aiguillage.ligne.voie, EST);
				utiliserVoie(&aiguillage.est.voieOuest, EST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.garage.voieTGV.mutex);
			pthread_mutex_unlock(&aiguillage.tunnel.voie.mutex);
			pthread_mutex_unlock(&aiguillage.ligne.voie.mutex);
			pthread_mutex_unlock(&aiguillage.est.voieOuest.mutex);
		}
		else // sens == OUEST
		{
			// Bloque le mutex de toutes les voies à réserver
			pthread_mutex_lock(&aiguillage.gare.voieD.mutex);
			if(peutAjouterTrain(&aiguillage.gare.voieD,  OUEST))
			{
				// Réserve toutes les voies nécessaires
				utiliserVoie(&aiguillage.gare.voieD, OUEST);

				// Libère la condition sur la voie actuelle
				retirerAttente(voie, t);
				pthread_cond_signal(&voie->condition[numTabType(t)]);
			}
			// Libère tous les mutex bloqués
			pthread_mutex_unlock(&aiguillage.gare.voieD.mutex);
		}
	}
	pthread_mutex_unlock(&voie->mutex);
}

void libererEst(Type t)
{
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
		pthread_mutex_lock(&aiguillage.garage.voieTGV.mutex);
		pthread_mutex_lock(&aiguillage.garage.voieGL.mutex);
		pthread_mutex_lock(&aiguillage.garage.voieMO.mutex);
		if(peutAjouterTrain(&aiguillage.ligne.voie, OUEST)
		&& peutAjouterTrain(&aiguillage.tunnel.voie, OUEST)
		&& peutAjouterTrain(voieGarage, OUEST)
		&& estPrioritaireSur(t, &aiguillage.garage.voieTGV, EST)
		&& estPrioritaireSur(t, &aiguillage.garage.voieGL, EST))
		{
			utiliserVoie(&aiguillage.ligne.voie, OUEST);
			utiliserVoie(&aiguillage.tunnel.voie, OUEST);
			utiliserVoie(voieGarage, OUEST);
			retirerAttente(voie, t);
			pthread_cond_signal(&voie->condition[numTabType(t)]);
		}
		pthread_mutex_unlock(&aiguillage.ligne.voie.mutex);
		pthread_mutex_unlock(&aiguillage.tunnel.voie.mutex);
		pthread_mutex_unlock(&aiguillage.garage.voieTGV.mutex);
		pthread_mutex_unlock(&aiguillage.garage.voieGL.mutex);
		pthread_mutex_unlock(&aiguillage.garage.voieMO.mutex);
	}
	pthread_mutex_unlock(&voie->mutex);
}

/****************************************************************/
/*************************** GET VOIE ***************************/
/****************************************************************/

/*
 * Retourne une voie en fonction de la position,
 * du type de train ainsi que de la direction du train
*/
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
		pthread_mutex_lock(&act->mutex);
		train->dort = true;
		// attends libération du suivant
		ajouterAttente(act, train->type);
		pthread_cond_wait(&act->condition[numTabType(train->type)], &act->mutex);
		train->dort = false;
		pthread_mutex_unlock(&act->mutex);
	}

	if(!peutUtiliser(next, train->direction))
	{
		// INTERBLOCAGE
		pthread_mutex_lock(&mutexEcriture);
		printf("INTERBLOCAGE : de %d en %d\n", train->id, train->position);
		pthread_mutex_unlock(&mutexEcriture);
		return -1;
	}

	// Libère les ressources précédentes
	pthread_mutex_lock(&act->mutex);
	train->position += suivant(train);
	libererVoie(act);
	pthread_mutex_unlock(&act->mutex);

	dort(TEMPS);
	return 0;
}

/*
* Affiche les détails de l'aiguillage
*/
void printAiguillage()
{
	pthread_mutex_lock(&mutexEcriture);
	fflush(stdout);
	// Pour vider l'écran à chaque fois :
	// printf("\033[2J");
	// printf("\033[1;1H");
	struct winsize w;
	int i, ecart, pos, nb=1, nbMax = 0, p0, totMax;
	p0 = 0;
	ecart = 20;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	totMax = w.ws_row-3;
	for(pos = p0; pos <= POS_EST; pos++)
	{
		nb = 1;
		// droite
		printf("\033[%dC", (pos-p0)*ecart);
		printPos(pos);
		for(i = 0;i<NB_THREAD; i++)
		{
			if(trains[i].position == pos && !arrive(&trains[i]))
			{
				if(++nb > totMax)
				{
					printf("\033[%dC       ...\n", (pos-p0)*ecart);
					break;
				}
				// droite
				printf("\033[%dC", (pos-p0)*ecart);
				printTrain(&trains[i]);
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
