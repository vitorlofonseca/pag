/*
 * gap.c
 * 
 * Copyright 2016 vitor <vitor@DESKTOP-370EE9O>
 * 
 * Este programa é um algoritmo genético aplicado a redes de computadores
 * que tem como objetivo decidir a melhor rota para um pacote percorrer.
 * Abaixo, temos algumas suposições para o programa no mínimo fazer sentido.
 * 
 * - Todos os n roteadores tem enlaces com os outros n-1 roteadores.
 * - O crossover será feito entre os números 1 e 3 | 2 e 4 e o ponto de corte será 4 e 2
 * - A quantidade de rotas (população inicial sempre deve ser par
 * - Neste programa, está sendo usado Elitismo em vez de mutação
 * - O ELITISMO SÓ ESTÁ SENDO REALIZADO NA ULTIMA ROTA
 * - O elitismo consiste em pegar o roteador com o melhor tempo da penúltima rota e clonar na última rota
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

/* ---------------------------------- MANIPULAÇÃO DA LISTA ------------------------------------*/

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
	rota *caminho = (rota*)malloc(sizeof(rota));			//alocando espaço na memória para a lista encadeada rota
	roteador *rtd = criarRoteador(hostOrigem, ultimoOcOrigem);				//criando o primeiro roteador com o ip origem
	caminho->primeiroRoteador = rtd;						//o primeiro roteador do caminho é o roteador criado
	caminho->tempoRota = 0;									//tempo da rota inicializado
	caminho->somaFitness = 0.0;
	return caminho;											//retorno do primeiro roteador
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
	rota *caminho = inicializaRota();						//inicializa a rota					
	
	int i=0;
	/* Essa parte serve para randomizar os roteadores entre o destino e o final */
	for (i=0 ; i<rand() % faixaIP + 3 ; i++){		
		char ultimoOc[3];					
		int ultimoOcInt = rand() % faixaIP + 3;				
		sprintf(ultimoOc, "%d", ultimoOcInt);
		char ip[tamanhoIP];
		strcpy (ip, "192.168.1.");
		strcat (ip, ultimoOc);
		
		if (detectarRepetidos (caminho, ip) == -1){ 					//Se já existir um roteador com o mesmo ip, não insira
			adicionarRoteador (caminho, ip, ultimoOcInt); 
		}
			
	}
	/* FIM DESSA PARTE */
	
	
	/* Essa parte serve para localizar o ultimo roteador aleatório, e amarrar o destino efetivo no final da rota */
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
	while (aux->tempoProx != -1){                                        //	CÁLCULO DO TEMPO TOTAL DA ROTA	
		caminho->tempoRota = caminho->tempoRota + aux->tempoProx; 		  //
		aux = aux->proxRtd;												//		
	} 																  // 
	
	/* FIM DESSA PARTE */
	
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
		printf ("Rota %d\n\n", i+1);
	
		roteador *aux = caminho[i]->primeiroRoteador;
	
		while (aux != NULL){
			printf ("IP: %s - Tempo para o proximo roteador: %dms - Binario do ultimo Octeto: %s - Fitness Salto: %f\n", aux->ip, aux->tempoProx, aux->ultimoOcBin, aux->fitness);
			aux = aux->proxRtd;
		}
		printf ("Tempo Total: %dms\n", caminho[i]->tempoRota);
		printf ("Fitness da Rota: %f\n", caminho[i]->somaFitness);
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

/* ---------------------------------- MANIPULAÇÃO DA LISTA ------------------------------------*/

/* ----------------------------------- ALGORÍTMO GENÉTICO -------------------------------------*/ 

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
	int crossovadas [qtdRotas];				//flag de controle pra saber quais rotas sofreram crossover
	int metodo = rand() % 3;
	
	for (i=0 ; i<qtdRotas ; i++)
		crossovadas [i] = 0;
	
	for (i=0 ; i<qtdRotas ; i++){
		if (metodo == 1){
			if (crossovadas[i] == 0){
				int pontoDeCorte = 1 + (rand() % menorRota);				
				printf ("PONTO DE CORTE %d - %d: %d\n", i+1, i+3, pontoDeCorte);
				if (menorRota == pontoDeCorte || pontoDeCorte > menorRota)				// \ Dessa forma, o ponto de corte nunca vai ser maior do que a menor rota.
					pontoDeCorte = 1;													// / E se for o ponto de corte vai ser depois do primeiro roteador
				
				/* Crossover Efetivo */
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
				printf ("PONTO DE CORTE %d - %d: %d\n", i+1, i+2, pontoDeCorte);
				if (menorRota == pontoDeCorte || pontoDeCorte > menorRota)				// \ Dessa forma, o ponto de corte nunca vai ser maior do que a menor rota.
					pontoDeCorte = 1;													// / E se for o ponto de corte vai ser depois do primeiro roteador
				
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
				/* Fim do Crossover Efetivo */
				crossovadas[i] = 1;
				crossovadas[i+1] = 1;
			}
		}
		
		if (metodo == 2){
			if (crossovadas[i] == 0){
				int pontoDeCorte = 1 + (rand() % menorRota);				
				printf ("PONTO DE CORTE %d - %d: %d\n", i+1, qtdRotas-i, pontoDeCorte);
				if (menorRota == pontoDeCorte || pontoDeCorte > menorRota)				// \ Dessa forma, o ponto de corte nunca vai ser maior do que a menor rota.
					pontoDeCorte = 1;													// / E se for o ponto de corte vai ser depois do primeiro roteador
				
				/* Crossover Efetivo */
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
	
	printf ("Deseja continuar o crossover de rotas? 1-sim, 0-nao ");
	scanf ("%d", &continua);
		
	int contGeracoes = 1;		//inicializando contador de gerações 
	
	while (continua == 1){
		
		printf ("Geracao %d:\n\n", contGeracoes); 
		fazerCrossover (caminho);
		fazerElitismo (caminho);
		removerRepetidos (caminho);
		atualizarTemposTotais (caminho);
		avaliarFitness (caminho);	
		impressao (caminho);
		contGeracoes++;
		
		printf ("Deseja continuar o crossover de rotas? 1-sim, 0-nao ");
		scanf ("%d", &continua);
		printf ("\n\n");
	}
	
	return 0;
}
