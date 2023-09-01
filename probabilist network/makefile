CC=g++ 
CFLAGS=-g -Wall -std=c++17

main_one_pool: main_one_pool.o 
	$(CC) $(CFLAGS) -o main_one_pool.exe main_one_pool.o 

main_one_pool.o : main_one_pool.cpp BinNeuron.h Synapse.h SynPool.h SubPool.h 
	$(CC) $(CFLAGS) -c main_one_pool.cpp  -I/usr/include/python2.7 -lpython2.7

