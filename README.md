# PAG (Protocolo em Algoritmo Genético)

Este programa tem por objetivo decidir a melhor rota entre um host origem e um host destino através de um algoritmo genético.

1 - Situação atual:
 - O algoritmo trabalha com lista encadeada.
 - Os endereços de cada roteador e seus respectivos tempos de transferência de pacote até o próximo roteador, são aleatórios.
 - Neste programa, está sendo usado Elitismo em vez de mutação (que consiste em pegar o roteador com o melhor tempo da penúltima rota e clonar na última rota)
 - O ELITISMO SÓ ESTÁ SENDO REALIZADO NA ULTIMA ROTA
 
2 - Metas:
 - Implantar o protocolo ICMP para trabalhar com rotas reais e suas possibilidades
 - Implementar um protocolo (PAG) de reservar recurso, para transferência de grandes arquivos pela melhor rota (decidida pelo algoritmo genético).
 
3 - Suposições para dar sentido ao algoritmo atual: 
 - Todos os n roteadores tem enlaces com os outros n-1 roteadores.
 - A quantidade de rotas (população inicial) sempre deve ser par
