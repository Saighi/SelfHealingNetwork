CC=g++
CFLAGS=-g -Wall 

rate_model_minimal_attractors: rate_model_minimal_attractors.o tools.o tools.cpp
	$(CC) $(CFLAGS) -o rate_model_minimal_attractors.exe rate_model_minimal_attractors.o tools.o

rate_model_minimal_attractors.o : rate_model_minimal_attractors.cpp Neuron.h Synapse.h tools.h tools.cpp
	$(CC) $(CFLAGS) -c rate_model_minimal_attractors.cpp -std=c++11 -I/usr/include/python2.7 -lpython2.7

tools.o : tools.h tools.cpp
	$(CC) $(CFLAGS) -c tools.cpp -std=c++11 -I/usr/include/python2.7 -lpython2.7