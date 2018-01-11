#include <stdio.h> 	// printf
#include <pthread.h>	// pthread
#include <semaphore.h>	// semaphores
#include <fcntl.h>     
#include <unistd.h> 	// sleep
#include <stdlib.h>	// srand
#include <time.h>	// time
// dans ce code nous avons utilisé des sémaphores non-nommés
/* Les stations */
#define Num_Stations 5
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4

/* Code des threads ""trains"" */

void* train_de_A_B_C_B_A(void* arg);//train1
void* train_de_A_B_D_C_B_A(void* arg);//train2
void* train_de_A_B_D_C_E_A(void* arg);//train3



/* Code des semaphores ""voies"" */
sem_t mutAB;
sem_t mutBA;
sem_t mutFifo; 

int nb_train = 0;

/*********************** Fonction permetant de nommer les segements de voies ferrées **************************/

void voie(int  ville_depart, int ville_arrivee)
{
	
    if  ( (ville_depart == A) && (ville_arrivee == B) )
		fprintf (stdout,"AB\n");     //La voie AB
	else
		if  ( (ville_depart == B) && (ville_arrivee == C) )
			fprintf (stdout,"BC\n");	// La voie BC
		else
            if  ( (ville_depart == C) && (ville_arrivee == B) )
			fprintf (stdout,"CB\n");	//La voie CB
			else
            if  ( (ville_depart == B) && (ville_arrivee == A) )
			fprintf (stdout,"BA\n");	//La voie BA
            else
            if  ( (ville_depart == B) && (ville_arrivee == D) )
			fprintf (stdout,"BD\n");	//La voie BD
            else
            if  ( (ville_depart == D) && (ville_arrivee == C) )
			fprintf (stdout,"DC\n");	//La voie DC
            else
            if  ( (ville_depart == C) && (ville_arrivee == E) )
			fprintf (stdout,"CE\n");	//La voie CE
            else
			fprintf (stdout,"EA\n");	// La voie EA


}


/************************************************ Gestion des voies *******************************************/

void deplacement_train_voieAB(int train,int ville_depart, int ville_arrivee)
{
	
	sem_wait(&mutFifo);  // file d'attente des trains des deux sens.  
	sem_wait(&mutAB);// bloquer les autres trains du sens "AB" pour protéger l'incrémentation de leurs nb_train
	nb_train = nb_train + 1;
	if(nb_train == 1)
		sem_wait(&mutBA);// bloquer le passage des trains du sens "BA" de rentrer à la voie(V_dp,V_ar)
	sem_post(&mutFifo);
	sem_post(&mutAB);
	/** Spécification du nom du train present sur la voie  **/
	if( train == 1)
	fprintf(stdout,"Le train ELBA est dans la voie ");
	else if (train == 2)
	fprintf(stdout,"Le train ELAO est dans la voie ");
	else
	fprintf(stdout,"Le train DEBA est dans la voie ");
	voie(ville_depart,ville_arrivee);

	/*********************************************************/

	sleep(rand() % 3); // Faire endormir le train sur le segment de voie ,pour un temps aléatroire de 1 à 3 secondes afin de simuler la durée de son trajet sur ce segment.

	/** Spécification du nom du train present sur la voie  **/
	if( train == 1)
	fprintf(stdout,"Le train ELBA  quitte  la voie ");
	else if (train == 2)
	fprintf(stdout,"Le train ELAO quitte la voie ");
	else
	fprintf(stdout,"Le train DEBA quitte la voie ");
	voie(ville_depart,ville_arrivee);

	/********************************************************/ 
	nb_train = nb_train - 1;
		if(nb_train == 0)
			sem_post(&mutBA);

	sem_post(&mutAB);

}

void deplacement_train_voieBA(int train,int ville_depart, int ville_arrivee)
{
	
	sem_wait(&mutFifo);// file d'attente des trains des deux sens. 
	sem_wait(&mutBA);  // bloquer les autres trains du sens "BA" pour protéger l'incrémentation de leurs nb_train
	nb_train = nb_train + 1;
	if(nb_train == 1)
		sem_wait(&mutAB);// bloquer le passage des trains du sens "AB" de rentrer à la voie(V_dp,V_ar)
	sem_post(&mutBA);
	sem_post(&mutFifo);

	/**Spécification du nom du train present sur la voie **/
	if( train == 1)
	fprintf(stdout,"Le train ELBA est dans la voie ");
	else if (train == 2)
	fprintf(stdout,"Le train ELAO est dans la voie ");
	else
	fprintf(stdout,"Le train DEBA est dans la voie ");
	voie(ville_depart,ville_arrivee);

	/*******************************************************/

	sleep(rand() % 3); // Faire endormir le train sur le segment de voie ,pour un temps aléatroire de 1 à 3 secondes afin de simuler la durée de son trajet sur ce segment.

	/** Spécification du nom du train present sur la voie */
	if( train == 1)
	fprintf(stdout,"Le train ELBA quitte la voie ");
	else if (train == 2)
	fprintf(stdout,"Le train ELAO quitte la voie ");
	else
	fprintf(stdout,"Le train DEBA quitte la voie ");
	voie(ville_depart,ville_arrivee);

	/***************************************************/
	sem_wait(&mutBA);
		nb_train = nb_train - 1;
		if(nb_train == 0)
			sem_post(&mutAB);

	sem_post(&mutBA);
}



/******************************** Géstion des parcours des trains *********************************************/

void* train_de_A_B_C_B_A(void* arg)
{
	FILE * fichier;
    	int train_de_A_B_C_B_A=1;
	long clk_clk=CLOCKS_PER_SEC;
    	clock_t t1 ,t2;
    	t1=clock();
   	time_t time_start;
 	time(&time_start);
 	time_t time_end;
 	time(&time_end);
 	fprintf(stdout,"\n               Début de service du Train ELBA :  %s \n",ctime(&time_start));
	int nbr = 0;
    while (nbr < 50) 
    {
		fprintf(stdout,"Le train ELBA veut s'engager sur la voie AB \n ");
		deplacement_train_voieAB(train_de_A_B_C_B_A,A, B);
		fprintf(stdout,"Le train ELBA veut s'engager sur la voie BC \n");
    		deplacement_train_voieAB(train_de_A_B_C_B_A,B,C);
		fprintf(stdout,"Le train ELBA veut s'engager sur la voie CB \n");
		deplacement_train_voieBA(train_de_A_B_C_B_A,C, B);
		fprintf(stdout,"Le train ELBA veut s'engager sur la voie BA \n");
		deplacement_train_voieBA(train_de_A_B_C_B_A,B, A);
        nbr++;
	}

	t2=clock();
	fprintf(stdout,"Le temps consommé (s)  par le train ELBA sur l'ensemble de ses trajets est :%lf S\n ",(double)(t2 - t1)/(double) clk_clk);
	fprintf(stdout,"Fin de service du Train ELBA : %s\n",ctime(&time_end));
	fichier = fopen("ResultatExecution.txt", "a+");

    if (fichier != NULL)

    {

        fprintf(fichier,"\n\nLe temps consommé par le train ELBA est  :%lf S (Semaphores) \n",(double)(t2 - t1)/(double) clk_clk); // Écriture dans le fichier

        fclose(fichier);

    }

	return NULL;

}

void* train_de_A_B_D_C_B_A(void* arg)
{   	FILE *fichier;
	int train_de_A_B_D_C_B_A =2;
	int nbr1=0;
	long clk_clk=CLOCKS_PER_SEC;
   	clock_t t1 ,t2;
    	t1=clock();
    	time_t time_start;
 	time(&time_start);
 	time_t time_end;
 	time(&time_end);
 	
 	fprintf(stdout,"\n               Début de service du Train  ELAO :%s \n\n ",ctime(&time_start));
    while(nbr1<50) 
    {
		fprintf(stdout,"Le train ELAO veut s'engager sur la voie AB \n");
        	deplacement_train_voieAB(train_de_A_B_D_C_B_A,A, B);
		fprintf(stdout,"Le train ELAO veut s'engager sur la voie BD \n");
		deplacement_train_voieAB(train_de_A_B_D_C_B_A,B, D);
		// voie unique respectée
		fprintf(stdout,"Le train ELAO veut s'engager sur la voie DC \n");
		deplacement_train_voieAB(train_de_A_B_D_C_B_A,D, C);
		fprintf(stdout,"Le train ELAO veut s'engager sur la voie CB \n");
		deplacement_train_voieBA(train_de_A_B_D_C_B_A,C, B);
		fprintf(stdout,"Le train ELAO  veut s'engager sur la voie BA \n");
		deplacement_train_voieBA(train_de_A_B_D_C_B_A,B, A);
        nbr1++;
	}

	t2=clock();
	fprintf(stdout,"Le temps consommé (s)  par le train ELAO sur l'ensemble de ses trajets est :  %lf S\n ",(double)(t2 - t1)/(double) clk_clk);
	fprintf(stdout,"Fin de service du Train ELAO (Semaphore): %s\n",ctime(&time_end));
	fichier = fopen("ResultatExecution.txt", "a+");

    if (fichier != NULL)

    {

        fprintf(fichier,"\n\nLe temps consommé par le train ELAO est  :%lf S (Semaphores) \n",(double)(t2 - t1)/(double) clk_clk); // Écriture dans le fichier

        fclose(fichier);

    }

	return NULL;
}


void* train_de_A_B_D_C_E_A(void* arg)
{    
	FILE * fichier;
	int train_de_A_B_D_C_E_A=3;
	int nbr2=0;
	long clk_clk=CLOCKS_PER_SEC;
        clock_t t1 ,t2;
        t1=clock();
    	time_t time_start;
 	time(&time_start);
 	time_t time_end;
 	time(&time_end);
 	
	fprintf(stdout,"\n               Début de service du Train DEBA : %s \n",ctime(&time_start));
    while (nbr2<50) 
    {
		fprintf(stdout,"Le train DEBA veut s'engager sur la voie AB \n");
        	deplacement_train_voieAB(train_de_A_B_D_C_E_A,A, B);
		// voie unique respectée
		fprintf(stdout,"Le train DEBA veut s'engager sur la voie BD \n");
		deplacement_train_voieAB(train_de_A_B_D_C_E_A,B, D);
		fprintf(stdout,"Le train DEBA veut s'engager sur la voie DC \n");
		deplacement_train_voieAB(train_de_A_B_D_C_E_A,D, C);
		fprintf(stdout,"Le train DEBA veut s'engager sur la voie CE \n");
		deplacement_train_voieAB(train_de_A_B_D_C_E_A,C, E);
		fprintf(stdout,"Le train DEBA veut s'engager sur la voie EA \n");
		deplacement_train_voieBA(train_de_A_B_D_C_E_A,E, A);
        nbr2++;
	}
	t2=clock();
	
    	fprintf(stdout,"Le temps consommé (s)  par le train DEBA sur l'ensemble de ses trajets est :  %lf S \n ",(double)(t2 - t1)/(double) clk_clk);
	fprintf(stdout,"Fin de service du Train  DEBA :  %s\n",ctime(&time_end));
	fichier = fopen("ResultatExecution.txt", "a+");

    if (fichier != NULL)

    {

        fprintf(fichier,"\n\nLe temps consommé par le train DEBA est  :%lf S (Semaphores) \n",(double)(t2 - t1)/(double) clk_clk); // Écriture dans le fichier

        fclose(fichier);

    }

	return NULL;

}

/************************************* Lancement du programme  ************************************/


int main(int argc, char* argv[])
{
	int i;
	pthread_t id[3];
	srand(time(NULL));

	/* Creer autant de trains que necessaire */
	
	sem_init(&mutAB,0, 1);
    	sem_init(&mutBA,0, 1);
    	sem_init(&mutFifo,0, 1);
	pthread_create(&id[0], NULL, train_de_A_B_C_B_A, NULL);
	pthread_create(&id[1], NULL, train_de_A_B_D_C_B_A, NULL);
	pthread_create(&id[2], NULL, train_de_A_B_D_C_E_A, NULL);

    for (i = 0; i < 3; i++) {
		pthread_join(id[i], NULL);
    }

	sem_destroy(&mutAB);
    	sem_destroy(&mutBA);
    	sem_destroy(&mutFifo); 	
	
    return 0;
}

