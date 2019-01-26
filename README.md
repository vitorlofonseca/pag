# Prodalg

This software has as objective to decide the best route between a source host and a destination host, through Genetic Algorithm, considering the overload in each network link.

1 - Actual Situation
 - The algorithm works with linked list
 - The address of each router is random. 
 - The router X transfer times to next routers are random
 - This algorithm is using elitism, instead mutation
 - The elitism is only being used in the last route
 
2 - Next Goals
 - Develop integration with ICMP protocol, to work with real routes and your latency possibilities
 - Develop integration with PAG protocol (network resource allocation), to big files transference in best route (decided by Genetic Algorithm)
 
3 - Considerations that give sense to atual algorithm
 - All of N routers have links with the N-1 other routers  
 - The routers quantity should initially be pair
