CC=g++ 
CFLAGS=-g -Wall -std=c++17

rate_model_minimal_attractors_large_pools: rate_model_minimal_attractors_large_pools.o tools.o tools.cpp
	$(CC) $(CFLAGS) -o rate_model_minimal_attractors_large_pools.exe rate_model_minimal_attractors_large_pools.o tools.o 

rate_model_minimal_attractors_large_pools.o : rate_model_minimal_attractors_large_pools.cpp Neuron.h Synapse.h tools.h tools.cpp
	$(CC) $(CFLAGS) -c rate_model_minimal_attractors_large_pools.cpp  -I/usr/include/python2.7 -lpython2.7

tools.o : tools.h tools.cpp
	$(CC) $(CFLAGS) -c tools.cpp -I/usr/include/python2.7 -lpython2.7