CC=g++ 
CFLAGS=-g -Wall -std=c++17

main_recurrent: main_recurrent.o 
	$(CC) $(CFLAGS) -o main_recurrent.exe main_recurrent.o 

main_recurrent.o : main_recurrent.cpp BinNeuron.h Synapse.h SynPool.h SubPool.h 
	$(CC) $(CFLAGS) -c main_recurrent.cpp  -I/usr/include/python2.7 -lpython2.7

