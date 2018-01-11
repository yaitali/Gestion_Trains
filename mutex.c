#include <stdio.h>   // printf
#include <pthread.h> // pthread
#include <unistd.h>  // sleep
#include <stdlib.h>  // srand
#include <time.h>    // time

/* Les stations */
#define Num_Stations 5
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4

/* Code des threads */

void* train_de_A_B_C_B_A(void* arg);//train1
void* train_de_A_B_D_C_B_A(void* arg);//train2
void* train_de_A_B_D_C_E_A(void* arg);//train3



/* Code des mutex ""voies"" */ 
pthread_mutex_t mutAB[Num_Stations][Num_Stations];
pthread_mutex_t mutBA[Num_Stations][Num_Stations];
pthread_mutex_t mutFifo;
pthread_cond_t cond;
int nb_train = 0;

/*********************** Fonction permetant de nommer les segements de voies ferrées **************************/

void voie(int  ville_depart, int ville_arrivee)
{ 
	
    if  ( (ville_depart == A) && (ville_arrivee == B) )
		fprintf (stdout,"AB\n");
	else
		if  ( (ville_depart == B) && (ville_arrivee == C) )
			fprintf (stdout,"BC\n");
		else
            if  ( (ville_depart == C) && (ville_arrivee == B) )
			fprintf (stdout,"CB\n");
			else
            if  ( (ville_depart == B) && (ville_arrivee == A) )
			fprintf (stdout,"BA\n");
            else
            if  ( (ville_depart == B) && (ville_arrivee == D) )
			fprintf (stdout,"BD\n");
            else
            if  ( (ville_depart == D) && (ville_arrivee == C) )
			fprintf (stdout,"DC\n");
            else
            if  ( (ville_depart == C) && (ville_arrivee == E) )
			fprintf (stdout,"CE\n");
            else
			fprintf (stdout,"EA\n");


}


/********************************************************** Gestion des vois **************************************/

void deplacement_train_voieAB(int train,int ville_depart, int ville_arrivee)
{	
       
	pthread_mutex_lock(&mutFifo);    // file d'attente des trains des deux sens.  
	pthread_mutex_lock(&mutAB[ville_depart][ville_arrivee]); // bloquer les autres trains du sens "AB" pour protéger l'incrémentation de leurs nb_train,verrouiller la section critique voie(V_dp,V_ar) 
	nb_train = nb_train + 1;
	if(nb_train == 1) 
		pthread_mutex_lock(&mutBA[ville_depart][ville_arrivee]);// bloquer le passage des trains du sens "BA" de rentrer à la voie(V_dp,V_ar),verrouiller la section critique voie(V_dp,V_ar)
	
	pthread_mutex_unlock(&mutAB[ville_depart][ville_arrivee]);// permet aux autres trains du sens "AB" de rentrer à la voie 
	pthread_mutex_unlock(&mutFifo); 
	/** Spécification du nom du train present sur la voie**/
	if( train == 1)
	fprintf(stdout,"Le train ELBA est dans la voie ");
	else if (train == 2)
	fprintf(stdout,"Le train ELAO est dans la voie ");
	else
	fprintf(stdout,"Le train DEBA est dans la voie ");
	voie(ville_depart,ville_arrivee);

	/******************************************************/

	sleep(rand() % 3); // Faire endormir le train sur le segment de voie ,pour un temps aléatroire de 1 à 3 secondes afin de simuler la durée de son trajet sur ce segment.

	/** Spécification du nom du train present sur la voie**/
	if( train == 1)
	fprintf(stdout,"Le train ELBA  quitte la voie ");
	else if (train == 2)
	fprintf(stdout,"Le train ELAO quitte la voie ");
	else
	fprintf(stdout,"Le train DEBA quitte la voie ");
	voie(ville_depart,ville_arrivee);

	/******************************************************/
	
	pthread_mutex_lock(&mutAB[ville_depart][ville_arrivee]);//  bloquer les autres trains du sens "AB" pour modifier le nb_train  à l'interieur de la voie(V_dp,V_ar)
	nb_train = nb_train - 1;
		if(nb_train == 0)	
			pthread_mutex_unlock(&mutBA[ville_depart][ville_arrivee]);
	pthread_mutex_unlock(&mutAB[ville_depart][ville_arrivee]);
	
	
}


void deplacement_train_voieBA(int train,int ville_depart, int ville_arrivee)
{	
	pthread_mutex_lock(&mutFifo);    // file d'attente des trains des deux sens 
	pthread_mutex_lock(&mutBA[ville_depart][ville_arrivee]);// bloquer les autres trains du sens "BA" pour protéger l'incrémentation de leurs nb_train,verrouiller la section critique voie(V_dp,V_ar)
	nb_train = nb_train + 1;
	if(nb_train == 1) 
		pthread_mutex_lock(&mutAB[ville_depart][ville_arrivee]); // bloquer le passage des trains du sens "AB" de rentrer à la voie(V_dp,V_ar),verrouiller la section critique voie(V_dp,V_ar)

	pthread_mutex_unlock(&mutBA[ville_depart][ville_arrivee]);// permet aux autres trains du sens "BA" de rentrer à la voie 
	pthread_mutex_unlock(&mutFifo);

	/** Spécification du nom du train present sur la voie **/
	if( train == 1)
	fprintf(stdout,"Le train ELBA est dans la voie ");
	else if (train == 2)
	fprintf(stdout,"Le train ELAO est dans la voie ");
	else
	fprintf(stdout,"Le train DEBA est dans la voie ");
	voie(ville_depart,ville_arrivee);

	/******************************************************/

	sleep(rand() % 3); // Faire endormir le train sur le segment de voie ,pour un temps aléatroire de 1 à 3 secondes afin de simuler la durée de son trajet sur ce segment.

	/** Spécification du nom du train present sur la voie  **/
	if( train == 1)
	fprintf(stdout,"Le train ELBA quitte la voie ");
	else if (train == 2)
	fprintf(stdout,"Le train ELAO quitte la voie ");
	else
	fprintf(stdout,"Le train DEBA quitte la voie ");
	voie(ville_depart,ville_arrivee);

	/********************************************************/
	pthread_mutex_lock(&mutBA[ville_depart][ville_arrivee]);//  bloquer les autres trains du sens "BA" pour modifier le nb_train  à l'interieur de la voie(V_dp,V_ar)
	nb_train = nb_train - 1;
		if(nb_train == 0)	
			pthread_mutex_unlock(&mutAB[ville_depart][ville_arrivee]);	        
	pthread_mutex_unlock(&mutBA[ville_depart][ville_arrivee]);
	
}

/******************************** Géstion des parcours des trains ***************************************************/


void* train_de_A_B_C_B_A(void* arg)
{	
	FILE* fichier = NULL;            //ouverture d'un ficher de sauvgarde de résultats 
    	int train_de_A_B_C_B_A=1;
	long clk_clk=CLOCKS_PER_SEC;
        clock_t t1 ,t2;
        t1=clock();                  //Récuperation du temps initial 
        time_t time_start;
 	time(&time_start); 
 	time_t time_end;
 	time(&time_end);
	fprintf(stdout,"\n               Début de service du Train ELBA :  %s \n",ctime(&time_start));
	int nbr=0;
        while (nbr<50) {                     //nombre d'execution du train pour son trajet
		fprintf(stdout,"Le train ELBA  veut s'engager sur la voie AB \n ");
		deplacement_train_voieAB(train_de_A_B_C_B_A,A, B);
		fprintf(stdout,"Le train ELBA veut s'engager sur la voie BC \n");
    		deplacement_train_voieAB(train_de_A_B_C_B_A,B,C);
		fprintf(stdout,"Le train ELBA veut s'engager sur la voie CB \n");
		deplacement_train_voieBA(train_de_A_B_C_B_A,C, B);
		fprintf(stdout,"Le train ELBA veut s'engager sur la voie BA \n");
		deplacement_train_voieBA(train_de_A_B_C_B_A,B, A);
	
        nbr++;
		}

	t2 = clock();    // Recuperation du temps final 
	
    	fprintf(stdout,"\nLe temps consommé (s)  par le train ELBA sur l'ensemble de ses trajets est :%lf S\n ",(double)(t2 - t1)/(double) clk_clk);  //Affichage du temps consommé par le train
	fprintf(stdout,"\nFin de service du Train ELBA : %s \n",ctime(&time_end));
	
   	 fichier = fopen("ResultatExecution.txt", "a+");     // récuperation des resultats dans le fichier "ResultatExecution.txt",avec un mode d'ouverture écriture à la fin

    if (fichier != NULL)

    {

        fprintf(fichier,"\n Le temps consommé par le train ELBA est  :%lf S (Mutex)\n",(double)(t2 - t1)/(double) clk_clk); // Écriture dans le fichier

        fclose(fichier);

    }

 



	return NULL;
	
}

void* train_de_A_B_D_C_B_A(void* arg)
{	FILE* fichier = NULL;           //ouverture d'un ficher de sauvgarde de résultats
	int train_de_A_B_D_C_B_A =2;      
	int nbr1=0;
	long clk_clk=CLOCKS_PER_SEC;
    	clock_t t1 ,t2;
    	t1=clock();                   //Récuperation du temps initial
    	time_t time_start;
 	time(&time_start);
 	time_t time_end;
 	time(&time_end);
	fprintf(stdout,"\n               Début de service du Train  ELAO :%s \n\n ",ctime(&time_start));
    while(nbr1<50)                    //nombre d'execution du train pour son trajet
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
	t2=clock();            // Recuperation du temps final 
	
    	printf("\nLe temps consommé (s)  par le train ELAO sur l'ensemble de ses trajets est :  %lf S\n ",(double)(t2 - t1)/(double) clk_clk); //Affichage du temps consommé par le train
	
	fprintf(stdout,"\nFin de service du Train ELAO : %s \n",ctime(&time_end));
	
	fichier = fopen("ResultatExecution.txt", "a+");  // récuperation des resultats dans le fichier "ResultatExecution.txt",avec un mode d'ouverture écriture à la fin

    if (fichier != NULL)

    {

        fprintf(fichier,"\n Le temps consommé par le train ELAO est  :%lf S (Mutex) \n",(double)(t2 - t1)/(double) clk_clk); // Écriture dans le fichier

        fclose(fichier);

    }

 
	return NULL;
}

void* train_de_A_B_D_C_E_A(void* arg)
{   	FILE* fichier = NULL;       //ouverture d'un ficher de sauvgarde de résultats
	int train_de_A_B_D_C_E_A=3;       
	int nbr2=0;
	long clk_clk=CLOCKS_PER_SEC;
        clock_t t1 ,t2;
        t1=clock();              //Récuperation du temps initial
    	time_t time_start;
 	time(&time_start);
 	time_t time_end;
 	time(&time_end);
	fprintf(stdout,"\n               Début de service du Train DEBA  %s \n",ctime(&time_start));		
        while (nbr2<50) {   //nombre d'execution du train pour son trajet
		fprintf(stdout,"Le train DEBA  veut s'engager sur la voie AB \n");
        	deplacement_train_voieAB(train_de_A_B_D_C_E_A,A, B);
		// voie unique respectée
		fprintf(stdout,"Le train DEBA  veut s'engager sur la voie BD \n");
		deplacement_train_voieAB(train_de_A_B_D_C_E_A,B, D);
		fprintf(stdout,"Le train DEBA  veut s'engager sur la voie DC \n");
		deplacement_train_voieAB(train_de_A_B_D_C_E_A,D, C);
		fprintf(stdout,"Le train DEBA  veut s'engager sur la voie CE \n");
		deplacement_train_voieAB(train_de_A_B_D_C_E_A,C, E);
		fprintf(stdout,"Le train DEBA   veut s'engager sur la voie EA \n");
		deplacement_train_voieBA(train_de_A_B_D_C_E_A,E, A);
	
        nbr2++;
		}
	t2=clock(); // Recuperation du temps final 
	
    	fprintf(stdout,"\nLe temps consommé (s)  par le train DEBA  sur l'ensemble de ses trajets est :  %lf S \n ",(double)(t2 - t1)/(double) clk_clk);//Affichage du temps consommé par le train
	fprintf(stdout,"\nFin de service du Train  DEBA  :  %s \n",ctime(&time_end));
	fichier = fopen("ResultatExecution.txt", "a+");  // récuperation des resultats dans le fichier "ResultatExecution.txt",avec un mode d'ouverture écriture à la fin


    if (fichier != NULL)

    {

        fprintf(fichier,"\n Le temps consommé par le train DEBA est  :%lf S (Mutex) \n",(double)(t2 - t1)/(double) clk_clk); // Écriture dans le fichier

        fclose(fichier);

    }
	return NULL;
	
}

/************************************* Lancement du programme  ************************************/



int main(int argc, char* argv[])
{
	int i;
	pthread_t id[3];
	srand(time(NULL));// initialisation de rand 

	/* Creer autant de trains que
		necessaire */

	pthread_mutex_init(&mutAB[Num_Stations][Num_Stations],0);
	pthread_mutex_init(&mutBA[Num_Stations][Num_Stations],0);
	pthread_mutex_init(&mutFifo,0);
	pthread_cond_init(&cond, NULL);
	pthread_create(&id[0], NULL, train_de_A_B_C_B_A, NULL);
	pthread_create(&id[1], NULL, train_de_A_B_D_C_B_A, NULL);
	pthread_create(&id[2], NULL, train_de_A_B_D_C_E_A, NULL);	
		
	
        for (i = 0; i < 3; i++) {
		pthread_join(id[i], NULL);
         }
	
	pthread_mutex_destroy(&mutFifo);
	pthread_mutex_destroy(&mutAB[Num_Stations][Num_Stations]);
	pthread_mutex_destroy(&mutBA[Num_Stations][Num_Stations]);
        return 0;
       
        

}



