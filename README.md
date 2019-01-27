# Prodalg

This project has as objective to decide the best route between a source host and a destination host, through Genetic Algorithm, considering the overload in each network link.

### Steps to use:

1 - Define how many route possibilities you want in dataStructureManipulation.routeQtd

2 - Run the algorithm

3 - In the image below, we can see many informations, like the route number, the router hop, the hop latency, and the total latency from source router to destination router (both defined in dataStructureManipulation.sourceHost and dataStructureManipulation.destinationHost)
![screenshot_1](https://user-images.githubusercontent.com/16262664/51794839-b7f1e980-21c1-11e9-9820-0f6414503b85.png)

4 - The faster route between all, will be used in real scenario

### Current Scenario
 - The algorithm works with linked list
 - The address of each router is random. 
 - The crossover will be done between the numbers 1 and 3 | 2 and 4 and the separation point will be 4 and 2
 - The router X transfer times to next routers are random
 - The elitism is only being used in the last route
 - The elitism consist in catch the router with the best latency in the penultimate route, and to clone in the last route
 
### Next Goals
 - Develop integration with ICMP protocol, to work with real routes and your latency possibilities
 - Develop integration with PAG protocol (network resource allocation), to big files transference in best route (decided by Genetic Algorithm)
 
### Considerations that give sense to atual algorithm
 - All of N routers have links with the N-1 other routers  
 - The routers quantity should initially be pair
