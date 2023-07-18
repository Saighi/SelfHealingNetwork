CC=g++
CFLAGS=-g -Wall 

rate_model_minimal_attractors: rate_model_minimal_attractors.o
	$(CC) $(CFLAGS) -o rate_model_minimal_attractors.exe rate_model_minimal_attractors.o

rate_model_minimal_attractors.o : rate_model_minimal_attractors.cpp Neuron.h Synapse.h 
	$(CC) $(CFLAGS) -c rate_model_minimal_attractors.cpp -std=c++11 -I/usr/include/python2.7 -lpython2.7