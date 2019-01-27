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

#define faixaIP 50							//192.168.1.faixaIP
#define tamanhoIP 15
#define timeout 500
#define qtdRotas 8
#define hostDestino "192.168.1.100"
#define hostOrigem "192.168.1.1"
#define qtdSelecao 4
#define ultimoOcDestino 100
#define ultimoOcOrigem 1

typedef struct roteador{
	char ip [tamanhoIP];
	struct roteador *proxRtd;
	int tempoProx;
	char ultimoOcBin [9];
	double fitness;
}roteador;

typedef struct rota{
	roteador *primeiroRoteador;
	int tempoRota;
	double somaFitness;
}rota;

/* ---------------------------------- LIST MANIPULATION ------------------------------------*/

void removerRepetidos (rota *caminho[]){
	int i;
	for (i=0 ; i<qtdRotas ; i++){
		roteador *varredor = caminho[i]->primeiroRoteador->proxRtd;
		roteador *comparador = varredor->proxRtd;
		while (varredor->proxRtd != NULL){
			while (comparador->proxRtd != NULL){
				if (strcmp (comparador->ip, varredor->ip) == 0){
					roteador *removedor = caminho[i]->primeiroRoteador->proxRtd;
					while (removedor->proxRtd != comparador){
						removedor = removedor->proxRtd;
					}
					removedor->proxRtd = comparador->proxRtd;
					free (comparador);
				}
				comparador = comparador->proxRtd;
			}
			varredor = varredor->proxRtd;
			comparador = varredor->proxRtd;
		}	
	}
}

roteador* criarRoteador (char ip [], int ultimoOcInt){
	roteador *no = (roteador*)malloc(sizeof(roteador));
	
	char ultimoOcChar [9];
	itoa (ultimoOcInt, ultimoOcChar, 2);
	
	strcpy (no->ip, ip);								
	no->proxRtd = NULL;	
	no->fitness = 0.0;								
	no->tempoProx = rand() % timeout + 20;				
	strcpy (no->ultimoOcBin, ultimoOcChar);
	return no;
}

rota* inicializaRota (){
	rota *caminho = (rota*)malloc(sizeof(rota));			//allocating space in memory to linked list
	roteador *rtd = criarRoteador(hostOrigem, ultimoOcOrigem);				//creating the first router with the origin-IP
	caminho->primeiroRoteador = rtd;						
	caminho->tempoRota = 0;									//route time initialized
	caminho->somaFitness = 0.0;
	return caminho;											//first router return
}

void adicionarRoteador (rota *caminho, char ip [], int ultimoOcInt){
	roteador *varredor = caminho->primeiroRoteador;
	roteador *aux = criarRoteador(ip, ultimoOcInt);
	
	while (varredor->proxRtd != NULL){
		varredor = varredor->proxRtd;
	}
	varredor->proxRtd = aux;
}

int detectarRepetidos (rota *caminho, char ip[]){
	
	roteador *varredor = caminho->primeiroRoteador;
	while (varredor->proxRtd != NULL){
		if (strcmp (varredor->ip, ip) == 0){
			return 0;
		}
		varredor = varredor->proxRtd;
	}
	if (strcmp (varredor->ip, ip) == 0){
			return 0;
	}
	return -1;
}


rota* montarRota (){
	rota *caminho = inicializaRota();						//initialize the route
	
	int i=0;
	/* Randomize the routers between the source and the destination */
	for (i=0 ; i<rand() % faixaIP + 3 ; i++){		
		char ultimoOc[3];					
		int ultimoOcInt = rand() % faixaIP + 3;				
		sprintf(ultimoOc, "%d", ultimoOcInt);
		char ip[tamanhoIP];
		strcpy (ip, "192.168.1.");
		strcat (ip, ultimoOc);
		
		if (detectarRepetidos (caminho, ip) == -1){ 					//If already exist a router with the same IP, doesn't insert
			adicionarRoteador (caminho, ip, ultimoOcInt); 
		}
			
	}
	
	/* Localizing the last random router, and linking the efective destination in the end route */
	roteador *aux = caminho->primeiroRoteador;					
	
	while (aux->proxRtd != NULL){ 
		aux = aux->proxRtd;
	}
	
	if (aux->proxRtd == NULL){
			roteador *ultimo = criarRoteador (hostDestino, ultimoOcDestino);
			ultimo->tempoProx = -1;
			aux->proxRtd = ultimo;
		}
		
	aux = caminho->primeiroRoteador;							 	   //
																		//
	while (aux->tempoProx != -1){                                        //	Calc of route total time
		caminho->tempoRota = caminho->tempoRota + aux->tempoProx; 		  //
		aux = aux->proxRtd;												//		
	} 																  // 
	
	return caminho;
}

rota* inicializa (){
	rota *caminho;
	caminho = montarRota();
	return caminho;
}

void impressao (rota *caminho[]){
	
	int i=0;
	for (i=0 ; i<qtdRotas ; i++){
		printf ("Route %d\n\n", i+1);
	
		roteador *aux = caminho[i]->primeiroRoteador;
	
		while (aux != NULL){
			printf ("IP: %s - Time to next router: %dms - Last octet binary: %s - Fitness Jump: %f\n", aux->ip, aux->tempoProx, aux->ultimoOcBin, aux->fitness);
			aux = aux->proxRtd;
		}
		printf ("Total Time: %dms\n", caminho[i]->tempoRota);
		printf ("Route Fitness: %f\n", caminho[i]->somaFitness);
		printf ("\n\n");
	}
}

void atualizarTemposTotais (rota *caminho[]){
	int i;
	for (i=0 ; i<qtdRotas ; i++){
		roteador *varredor = caminho[i]->primeiroRoteador;
		caminho[i]->tempoRota = 0;
		while (varredor->proxRtd != NULL){
			caminho[i]->tempoRota = caminho[i]->tempoRota + varredor->tempoProx;	
			varredor = varredor->proxRtd;
		}
	}
}

/* ---------------------------------- LIST MANIPULATION ------------------------------------*/

/* ----------------------------------- GENETIC ALGORITHM -------------------------------------*/ 

void avaliarFitness (rota *caminho[]){ 
	int i;
	for (i=0 ; i<qtdRotas ; i++){
		roteador *varredor = caminho[i]->primeiroRoteador;
		caminho[i]->somaFitness = 0;
		
		while (varredor->proxRtd != NULL){
			varredor->fitness = 1.0 / varredor->tempoProx;
			varredor = varredor->proxRtd;
		}
		caminho[i]->somaFitness = 1.0/caminho[i]->tempoRota;
	}
	
}

void fazerElitismo (rota *caminho[]){
	roteador *varredor = caminho[qtdRotas-2]->primeiroRoteador->proxRtd;			
	roteador *maisRapido = varredor;
	float menorTempo = varredor->tempoProx;
	
	while (varredor->proxRtd != NULL){
		if (menorTempo > varredor->tempoProx){
			menorTempo = varredor->tempoProx;
			maisRapido = varredor;
		}
		varredor = varredor->proxRtd;
	}
	
	varredor = caminho [qtdRotas-1]->primeiroRoteador->proxRtd;						
	roteador *maisLento = varredor;
	float maiorTempo = varredor->tempoProx;
	
	while (varredor->proxRtd != NULL){
		if (maiorTempo < varredor->tempoProx){
			maiorTempo = varredor->tempoProx;
			maisLento = varredor;
		}
		varredor = varredor->proxRtd;
	}
	
	strcpy (maisLento->ip, maisRapido->ip);
	maisLento->tempoProx = maisRapido->tempoProx;
	strcpy (maisLento->ultimoOcBin, maisRapido->ultimoOcBin);
	maisLento->fitness = maisRapido->fitness;
}

void fazerCrossover (rota *caminho[]){
	
	int tamanhoRotas [qtdRotas], i;
	
	for (i=0 ; i<qtdRotas ; i++){
		roteador *aux = caminho[i]->primeiroRoteador;
		tamanhoRotas[i] = 0;
		while (aux!=NULL){
			tamanhoRotas[i]++;
			aux = aux->proxRtd;
		}
	}
	
	int menorRota = tamanhoRotas[0];			
	
	for (i=0 ; i<qtdRotas ; i++){
		if (menorRota>tamanhoRotas[i])
			menorRota = tamanhoRotas[i];
	}
	
	srand (time(0));
	int crossovadas [qtdRotas];				//control flag to know which routes was crossoved
	int metodo = rand() % 3;
	
	for (i=0 ; i<qtdRotas ; i++)
		crossovadas [i] = 0;
	
	for (i=0 ; i<qtdRotas ; i++){
		if (metodo == 1){
			if (crossovadas[i] == 0){
				int pontoDeCorte = 1 + (rand() % menorRota);				
				printf ("SEPARATION POINT %d - %d: %d\n", i+1, i+3, pontoDeCorte);
				if (menorRota == pontoDeCorte || pontoDeCorte > menorRota)				// \ This way, the separation point never will be greater than the smaller route
					pontoDeCorte = 1;													// / And if be, the separation point will be after first router
				
				/* Crossover in fact */
				roteador *varredorRota1 = caminho[i]->primeiroRoteador;
				roteador *varredorRota2 = caminho[i+2]->primeiroRoteador;
				
				int j;
				for (j=0 ; j<pontoDeCorte-1 ; j++){
					varredorRota1 = varredorRota1->proxRtd;
					varredorRota2 = varredorRota2->proxRtd;
				}
				roteador *aux = criarRoteador ("0.0.0.0", 0);
				
				aux->proxRtd = varredorRota1->proxRtd;
				varredorRota1->proxRtd = varredorRota2->proxRtd;
				varredorRota2->proxRtd = aux->proxRtd;
				/* Fim do Crossover Efetivo */
				crossovadas[i] = 1;
				crossovadas[i+2] = 1;
			}
		}
		
		if (metodo == 0){
			if (crossovadas[i] == 0){
				int pontoDeCorte = 1 + (rand() % menorRota);				
				printf ("SEPARATION POINT %d - %d: %d\n", i+1, i+2, pontoDeCorte);
				if (menorRota == pontoDeCorte || pontoDeCorte > menorRota)				// \ This way, the separation point never will be greater than the smaller route
					pontoDeCorte = 1;													// / And if be, the separation point will be after first router
				
				/* Crossover Efetivo */
				roteador *varredorRota1 = caminho[i]->primeiroRoteador;
				roteador *varredorRota2 = caminho[i+1]->primeiroRoteador;
				
				int j;
				for (j=0 ; j<pontoDeCorte-1 ; j++){
					varredorRota1 = varredorRota1->proxRtd;
					varredorRota2 = varredorRota2->proxRtd;
				}
				roteador *aux = criarRoteador ("0.0.0.0", 0);
				
				aux->proxRtd = varredorRota1->proxRtd;
				varredorRota1->proxRtd = varredorRota2->proxRtd;
				varredorRota2->proxRtd = aux->proxRtd;
				/* End of Crossover in fact */
				crossovadas[i] = 1;
				crossovadas[i+1] = 1;
			}
		}
		
		if (metodo == 2){
			if (crossovadas[i] == 0){
				int pontoDeCorte = 1 + (rand() % menorRota);				
				printf ("SEPARATION POINT %d - %d: %d\n", i+1, qtdRotas-i, pontoDeCorte);
				if (menorRota == pontoDeCorte || pontoDeCorte > menorRota)				// \ This way, the separation point never will be greater than the smaller route
					pontoDeCorte = 1;													// / And if be, the separation point will be after first router
				
				/* Crossover in fact */
				roteador *varredorRota1 = caminho[i]->primeiroRoteador;
				roteador *varredorRota2 = caminho[qtdRotas-i-1]->primeiroRoteador;
				
				int j;
				for (j=0 ; j<pontoDeCorte-1 ; j++){
					varredorRota1 = varredorRota1->proxRtd;
					varredorRota2 = varredorRota2->proxRtd;
				}
				roteador *aux = criarRoteador ("0.0.0.0", 0);
				
				aux->proxRtd = varredorRota1->proxRtd;
				varredorRota1->proxRtd = varredorRota2->proxRtd;
				varredorRota2->proxRtd = aux->proxRtd;
				/* Fim do Crossover Efetivo */
				crossovadas[i] = 1;
				crossovadas[qtdRotas-i-1] = 1;
			}
		}
		
	}
	printf ("\n\n");
}

/* ----------------------------------- ALGORÍTMO GENÉTICO -------------------------------------*/ 


int main (){
	int continua=1;
	
	/* INICIALIZANDO A POPULAÇÃO DE ROTAS */
	rota *caminho [qtdRotas];
	
	int i=0;
	for (i=0 ; i<qtdRotas ; i++){
		caminho[i] = inicializa();
	}
	/* FIM DA INICIALIZAÇÃO */
	
	avaliarFitness (caminho);	
	impressao (caminho);
	
	printf ("Do you want to continue route crossover? 1-yes, 0-no ");
	scanf ("%d", &continua);
		
	int contGeracoes = 1;		//inicializando contador de gerações 
	
	while (continua == 1){
		
		printf ("Generation %d:\n\n", contGeracoes); 
		fazerCrossover (caminho);
		fazerElitismo (caminho);
		removerRepetidos (caminho);
		atualizarTemposTotais (caminho);
		avaliarFitness (caminho);	
		impressao (caminho);
		contGeracoes++;
		
		printf ("Do you want to continue route crossover? 1-yes, 0-no ");
		scanf ("%d", &continua);
		printf ("\n\n");
	}
	
	return 0;
}
