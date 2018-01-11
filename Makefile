compiler1:mutex.c
	gcc  -Wall -o mutex.x   mutex.c -lpthread
executer1: 
	./mutex.x

compiler2:semaphore.c
	gcc  -Wall -o semaphore.x   semaphore.c -lpthread
executer2: 
	./semaphore.x

compiler3:verrous.c
	gcc  -Wall -o verrous.x   verrous.c -lpthread
executer3: 
	./verrous.x

