#!/bin/bash#!/bin/bash
gcc semaphore.c -Wall -o semaphore -lpthread
gcc verrous.c -Wall -o verrou -lpthread
gcc mutex.c -Wall -o mutex -lpthread

./semaphore
./verrou
./mutex 
