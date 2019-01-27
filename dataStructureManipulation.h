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
			printf ("IP: %s - Latency to next router: %dms - Last octet binary: %s - Fitness Jump: %f\n", aux->ip, aux->nextTime, aux->lastBinOc, aux->fitness);
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
