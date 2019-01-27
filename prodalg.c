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
#include "dataStructureManipulation.h"
#include "geneticAlgorithm.h"


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
