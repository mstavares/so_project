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

//#define NUMBER_OF_THREADS 1
#define NUMBER_OF_PIPES 4


transaction_t* waiting_list[NUMBER_OF_PIPES];

#define NUMBER_OF_THREADS 3
int array[4];

int compare(int a, int b) {
	if(a < b) {
		return 1;
	} else {
		return 0;
	}
}


/*
int waiting_algorithm() {
	int val;
	if(compare(waiting_list[0]->timestamp, waiting_list[1]->timestamp)) {
		if(compare(waiting_list[0]->timestamp, waiting_list[2]->timestamp)) {
			if(compare(waiting_list[0]->timestamp, waiting_list[3]->timestamp)) {
				val = 0;
			} else {
				val = 3;
			}
		} else {
			if(compare(waiting_list[2]->timestamp, waiting_list[3]->timestamp)) {
				val = 2;
			} else {
				val = 3;
			}
		}
	} else {
		if(compare(waiting_list[1]->timestamp, waiting_list[2]->timestamp)) {
			if(compare(waiting_list[1]->timestamp, waiting_list[3]->timestamp)) {
				val = 1;
			} else {
				val = 3;
			}
		} else {
			if(compare(waiting_list[2]->timestamp, waiting_list[3]->timestamp)) {
				val = 2;
			} else {
				val = 3;
			}
		}
	}
	return val;
}
*/


int waiting_algorithm() {
	int val;
	if(compare(array[0], array[1])) {
		if(compare(array[0], array[2])) {
			if(compare(array[0], array[3])) {
				val = 0;
			} else {
				val = 3;
			}
		} else {
			if(compare(array[2], array[3])) {
				val = 2;
			} else {
				val = 3;
			}
		}
	} else {
		if(compare(array[1], array[2])) {
			if(compare(array[1], array[3])) {
				val = 1;
			} else {
				val = 3;
			}
		} else {
			if(compare(array[2], array[3])) {
				val = 2;
			} else {
				val = 3;
			}
		}
	}
	return val;
}

void *readOrders() {
	int i;
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
		
		fread(transaction, sizeof(transaction_t), 1, fifos[i]);
		//printf("%d: %s \n", i, print_transaction(transaction));

	}
	
	//fclose(r);
}

int randomize() {
	return rand() % 20;
}

void teste_algoritmo() {
	int i;
	for(i = 0; i < 4; i++) {
		array[i] = randomize();
	}
	printf("MENOR: %d \n", waiting_algorithm());
}

void ultima() {
	int i;
	teste_algoritmo();
	printf("---------------------\n");
	for(i = 0; i < 4; i++) {
		printf("%d -> %d \n", i, array[i]);
	}
}

int main(int argc, char* argv[]) {
	int i;
	pthread_t threads[NUMBER_OF_THREADS];
	
	for(i = 0; i < 5; i++) {
		ultima();
    }	
    
    	
    	
    	
	/*
	for(i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_create(&threads[i], NULL, readOrders, NULL);
	}

    for(i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_join(threads[i], NULL);
    }
    */
}
