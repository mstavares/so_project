#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include "transaction.h"

#define NUMBER_OF_THREADS 1
#define NUMBER_OF_PIPES 4


void *readOrders(void *f) {
	int i;
	//char fifo[8];
	FILE *fifos[4];
	
	
	for (i = 0; i < NUMBER_OF_PIPES; i++) {
		char fifo[8];
		snprintf(fifo, sizeof(fifo), "fifo%d", i);
		fifos[i] = fopen(fifo, "rb");
		printf("%s \n", fifo);
	}
	
	
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	for(i = 0; ; i++) {
		if(i == 4) {
			i = 0;
		}
				
		sleep(1);
		fread(transaction, sizeof(transaction_t), 1, fifos[i]);
		printf("%d: %s \n", i, print_transaction(transaction));
		
		/*
		fread(transaction, sizeof(transaction_t), 1, fifos[1]);
		printf("%d: %s \n", 1, print_transaction(transaction));

		fread(transaction, sizeof(transaction_t), 1, fifos[2]);
		printf("%d: %s \n", 2, print_transaction(transaction));
		
		fread(transaction, sizeof(transaction_t), 1, fifos[3]);
		printf("%d: %s \n", 3, print_transaction(transaction));
		*/
	}
	
	//fclose(r);
}


/*
void* readOrders(void* fifo) {
	int i;
	char *ficheiro = (char*) fifo;
	printf("recebe: %s \n", ficheiro);
	
	FILE *r = fopen(ficheiro, "rb");
	printf("recebe22: \n");
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	for(;;) {
		sleep(1);
		fread(transaction, sizeof(transaction_t), 1, r);
		printf("%s: %s \n",ficheiro, print_transaction(transaction));
	}
	fclose(r);
}
*/

int main(int argc, char* argv[]) {
	int i;
	pthread_t threads[NUMBER_OF_THREADS];
	
	pthread_create(&threads[0], NULL, readOrders, (void*) "fifo0");
	pthread_join(threads[0], NULL);
	
	/*
	for(i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_create(&threads[i], NULL, readOrders, (void*) "teste");
	}

    for(i = 0; i < NUMBER_OF_THREADS; i++) {
    printf("teste");
		pthread_join(threads[i], NULL);
    }
	*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}
