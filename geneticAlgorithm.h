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
