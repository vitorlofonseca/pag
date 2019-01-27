/*
 * prodalg.c
 * 
 * Copyright 2016 vitor <vitor@DESKTOP-370EE9O>
 * 
 * This software is an Genetic Algorithm applied to computers network. 
 * It decide the best route where a network packet should go through. 
 * Below, we have some suppositions, to the script to make sense.
 * 
 * - All of N routers have links with the N-1 other routers
 * - The crossover will be done between the numbers 1 and 3 | 2 and 4 and the separation point will be 4 and 2
 * - The routes quantity should be pair
 * - The elitism is only being used in the last route
 * - The elitism consist in catch the router with the best latency in the penultimate route, and to clone in the last route
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define IPRange 50							//192.168.1.IPRange
#define IPSize 15
#define timeout 500
#define routeQtd 8
#define destinationHost "192.168.1.100"
#define sourceHost "192.168.1.1"
#define selectionQtd 4
#define lastDestinationOc 100
#define lastSourceOc 1

typedef struct router{
	char ip [IPSize];
	struct router *nextRtd;
	int nextTime;
	char lastBinOc [9];
	double fitness;
}router;

typedef struct route{
	router *firstRouter;
	int routeTime;
	double fitnessSum;
}route;

/* ---------------------------------- LIST MANIPULATION ------------------------------------*/

void removeRepeated (route *way[]){
	int i;
	for (i=0 ; i<routeQtd ; i++){
		router *iterator = way[i]->firstRouter->nextRtd;
		router *comparator = iterator->nextRtd;
		while (iterator->nextRtd != NULL){
			while (comparator->nextRtd != NULL){
				if (strcmp (comparator->ip, iterator->ip) == 0){
					router *remover = way[i]->firstRouter->nextRtd;
					while (remover->nextRtd != comparator){
						remover = remover->nextRtd;
					}
					remover->nextRtd = comparator->nextRtd;
					free (comparator);
				}
				comparator = comparator->nextRtd;
			}
			iterator = iterator->nextRtd;
			comparator = iterator->nextRtd;
		}	
	}
}

router* createRouter (char ip [], int lastIntOc){
	router *no = (router*)malloc(sizeof(router));
	
	char lastCharOc [9];
	itoa (lastIntOc, lastCharOc, 2);
	
	strcpy (no->ip, ip);								
	no->nextRtd = NULL;	
	no->fitness = 0.0;								
	no->nextTime = rand() % timeout + 20;				
	strcpy (no->lastBinOc, lastCharOc);
	return no;
}

route* initializeRoute (){
	route *way = (route*)malloc(sizeof(route));			//allocating space in memory to linked list
	router *rtd = createRouter(sourceHost, lastSourceOc);				//creating the first router with the origin-IP
	way->firstRouter = rtd;						
	way->routeTime = 0;									//route time initialized
	way->fitnessSum = 0.0;
	return way;											//first router return
}

void addRouter (route *way, char ip [], int lastIntOc){
	router *iterator = way->firstRouter;
	router *aux = createRouter(ip, lastIntOc);
	
	while (iterator->nextRtd != NULL){
		iterator = iterator->nextRtd;
	}
	iterator->nextRtd = aux;
}

int detectRepeated (route *way, char ip[]){
	
	router *iterator = way->firstRouter;
	while (iterator->nextRtd != NULL){
		if (strcmp (iterator->ip, ip) == 0){
			return 0;
		}
		iterator = iterator->nextRtd;
	}
	if (strcmp (iterator->ip, ip) == 0){
			return 0;
	}
	return -1;
}


route* mountRoute (){
	route *way = initializeRoute();						//initialize the route
	
	int i=0;
	/* Randomize the routers between the source and the destination */
	for (i=0 ; i<rand() % IPRange + 3 ; i++){		
		char lastOc[3];					
		int lastIntOc = rand() % IPRange + 3;				
		sprintf(lastOc, "%d", lastIntOc);
		char ip[IPSize];
		strcpy (ip, "192.168.1.");
		strcat (ip, lastOc);
		
		if (detectRepeated (way, ip) == -1){ 					//If already exist a router with the same IP, doesn't insert
			addRouter (way, ip, lastIntOc); 
		}
			
	}
	
	/* Localizing the last random router, and linking the efective destination in the end route */
	router *aux = way->firstRouter;					
	
	while (aux->nextRtd != NULL){ 
		aux = aux->nextRtd;
	}
	
	if (aux->nextRtd == NULL){
			router *last = createRouter (destinationHost, lastDestinationOc);
			last->nextTime = -1;
			aux->nextRtd = last;
		}
		
	aux = way->firstRouter;							 	   //
																		//
	while (aux->nextTime != -1){                                        //	Calc of route total time
		way->routeTime = way->routeTime + aux->nextTime; 		  //
		aux = aux->nextRtd;												//		
	} 																  // 
	
	return way;
}

route* initialize (){
	route *way;
	way = mountRoute();
	return way;
}

void show (route *way[]){
	
	int i=0;
	for (i=0 ; i<routeQtd ; i++){
		printf ("Route %d\n\n", i+1);
	
		router *aux = way[i]->firstRouter;
	
		while (aux != NULL){
			printf ("IP: %s - Time to next router: %dms - Last octet binary: %s - Fitness Jump: %f\n", aux->ip, aux->nextTime, aux->lastBinOc, aux->fitness);
			aux = aux->nextRtd;
		}
		printf ("Total Time: %dms\n", way[i]->routeTime);
		printf ("Route Fitness: %f\n", way[i]->fitnessSum);
		printf ("\n\n");
	}
}

void updateTotalLatencies (route *way[]){
	int i;
	for (i=0 ; i<routeQtd ; i++){
		router *iterator = way[i]->firstRouter;
		way[i]->routeTime = 0;
		while (iterator->nextRtd != NULL){
			way[i]->routeTime = way[i]->routeTime + iterator->nextTime;	
			iterator = iterator->nextRtd;
		}
	}
}

/* ---------------------------------- LIST MANIPULATION ------------------------------------*/

/* ----------------------------------- GENETIC ALGORITHM -------------------------------------*/ 

void evaluateFitness (route *way[]){ 
	int i;
	for (i=0 ; i<routeQtd ; i++){
		router *iterator = way[i]->firstRouter;
		way[i]->fitnessSum = 0;
		
		while (iterator->nextRtd != NULL){
			iterator->fitness = 1.0 / iterator->nextTime;
			iterator = iterator->nextRtd;
		}
		way[i]->fitnessSum = 1.0/way[i]->routeTime;
	}
	
}

void makeElitism (route *way[]){
	router *iterator = way[routeQtd-2]->firstRouter->nextRtd;			
	router *faster = iterator;
	float smallerTime = iterator->nextTime;
	
	while (iterator->nextRtd != NULL){
		if (smallerTime > iterator->nextTime){
			smallerTime = iterator->nextTime;
			faster = iterator;
		}
		iterator = iterator->nextRtd;
	}
	
	iterator = way [routeQtd-1]->firstRouter->nextRtd;						
	router *slowler = iterator;
	float greaterTime = iterator->nextTime;
	
	while (iterator->nextRtd != NULL){
		if (greaterTime < iterator->nextTime){
			greaterTime = iterator->nextTime;
			slowler = iterator;
		}
		iterator = iterator->nextRtd;
	}
	
	strcpy (slowler->ip, faster->ip);
	slowler->nextTime = faster->nextTime;
	strcpy (slowler->lastBinOc, faster->lastBinOc);
	slowler->fitness = faster->fitness;
}

void makeCrosover (route *way[]){
	
	int routesSize [routeQtd], i;
	
	for (i=0 ; i<routeQtd ; i++){
		router *aux = way[i]->firstRouter;
		routesSize[i] = 0;
		while (aux!=NULL){
			routesSize[i]++;
			aux = aux->nextRtd;
		}
	}
	
	int smallerRoute = routesSize[0];			
	
	for (i=0 ; i<routeQtd ; i++){
		if (smallerRoute>routesSize[i])
			smallerRoute = routesSize[i];
	}
	
	srand (time(0));
	int crossoved [routeQtd];				//control flag to know which routes was crossoved
	int method = rand() % 3;
	
	for (i=0 ; i<routeQtd ; i++)
		crossoved [i] = 0;
	
	for (i=0 ; i<routeQtd ; i++){
		if (method == 1){
			if (crossoved[i] == 0){
				int separationPoint = 1 + (rand() % smallerRoute);				
				printf ("SEPARATION POINT %d - %d: %d\n", i+1, i+3, separationPoint);
				if (smallerRoute == separationPoint || separationPoint > smallerRoute)				// \ This way, the separation point never will be greater than the smaller route
					separationPoint = 1;													// / And if be, the separation point will be after first router
				
				/* Crossover in fact */
				router *iteratorRoute1 = way[i]->firstRouter;
				router *iteratorRoute2 = way[i+2]->firstRouter;
				
				int j;
				for (j=0 ; j<separationPoint-1 ; j++){
					iteratorRoute1 = iteratorRoute1->nextRtd;
					iteratorRoute2 = iteratorRoute2->nextRtd;
				}
				router *aux = createRouter ("0.0.0.0", 0);
				
				aux->nextRtd = iteratorRoute1->nextRtd;
				iteratorRoute1->nextRtd = iteratorRoute2->nextRtd;
				iteratorRoute2->nextRtd = aux->nextRtd;
				/* Fim do Crossover Efetivo */
				crossoved[i] = 1;
				crossoved[i+2] = 1;
			}
		}
		
		if (method == 0){
			if (crossoved[i] == 0){
				int separationPoint = 1 + (rand() % smallerRoute);				
				printf ("SEPARATION POINT %d - %d: %d\n", i+1, i+2, separationPoint);
				if (smallerRoute == separationPoint || separationPoint > smallerRoute)				// \ This way, the separation point never will be greater than the smaller route
					separationPoint = 1;													// / And if be, the separation point will be after first router
				
				/* Crossover Efetivo */
				router *iteratorRoute1 = way[i]->firstRouter;
				router *iteratorRoute2 = way[i+1]->firstRouter;
				
				int j;
				for (j=0 ; j<separationPoint-1 ; j++){
					iteratorRoute1 = iteratorRoute1->nextRtd;
					iteratorRoute2 = iteratorRoute2->nextRtd;
				}
				router *aux = createRouter ("0.0.0.0", 0);
				
				aux->nextRtd = iteratorRoute1->nextRtd;
				iteratorRoute1->nextRtd = iteratorRoute2->nextRtd;
				iteratorRoute2->nextRtd = aux->nextRtd;
				/* End of Crossover in fact */
				crossoved[i] = 1;
				crossoved[i+1] = 1;
			}
		}
		
		if (method == 2){
			if (crossoved[i] == 0){
				int separationPoint = 1 + (rand() % smallerRoute);				
				printf ("SEPARATION POINT %d - %d: %d\n", i+1, routeQtd-i, separationPoint);
				if (smallerRoute == separationPoint || separationPoint > smallerRoute)				// \ This way, the separation point never will be greater than the smaller route
					separationPoint = 1;													// / And if be, the separation point will be after first router
				
				/* Crossover in fact */
				router *iteratorRoute1 = way[i]->firstRouter;
				router *iteratorRoute2 = way[routeQtd-i-1]->firstRouter;
				
				int j;
				for (j=0 ; j<separationPoint-1 ; j++){
					iteratorRoute1 = iteratorRoute1->nextRtd;
					iteratorRoute2 = iteratorRoute2->nextRtd;
				}
				router *aux = createRouter ("0.0.0.0", 0);
				
				aux->nextRtd = iteratorRoute1->nextRtd;
				iteratorRoute1->nextRtd = iteratorRoute2->nextRtd;
				iteratorRoute2->nextRtd = aux->nextRtd;
				/* Efective crossover end */
				crossoved[i] = 1;
				crossoved[routeQtd-i-1] = 1;
			}
		}
		
	}
	printf ("\n\n");
}

/* ----------------------------------- GENETIC ALGORITHM -------------------------------------*/ 


int main (){
	int continueExecution=1;
	
	/* INITIALIZING THE ROUTES POPULATION */
	route *way [routeQtd];
	
	int i=0;
	for (i=0 ; i<routeQtd ; i++){
		way[i] = initialize();
	}
	/* INITIALIZATION ENDS */
	
	evaluateFitness (way);	
	show (way);
	
	printf ("Do you want to continue route crossover? 1-yes, 0-no ");
	scanf ("%d", &continueExecution);
		
	int contGeracoes = 1;		//initializing generations counter
	
	while (continueExecution == 1){
		
		printf ("Generation %d:\n\n", contGeracoes); 
		makeCrosover (way);
		makeElitism (way);
		removeRepeated (way);
		updateTotalLatencies (way);
		evaluateFitness (way);	
		show (way);
		contGeracoes++;
		
		printf ("Do you want to continue route crossover? 1-yes, 0-no ");
		scanf ("%d", &continueExecution);
		printf ("\n\n");
	}
	
	return 0;
}
