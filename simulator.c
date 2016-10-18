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

// porque?
static transaction_t* waiting_list[NUMBER_OF_PIPES];
static int ponteiro;

int compare(time_t a, time_t b) {
	if(a < b) {
		return 1;
	} else {
		return 0;
	}
}


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


void *readOrders() {
	int i;
	FILE *fifos[4];
		
	for (i = 0; i < NUMBER_OF_PIPES; i++) {
		char fifo[8];
		snprintf(fifo, sizeof(fifo), "fifo%d", i);
		fifos[i] = fopen(fifo, "rb");
		printf("%s \n", fifo);
	}
	

	
	for(i = 0; i < 4; i++) {
		sleep(1);
		waiting_list[i] = (transaction_t *) malloc(sizeof(transaction_t));	
		fread(waiting_list[i], sizeof(transaction_t), 1, fifos[i]);
		printf("%d: %s \n",i, print_transaction(waiting_list[i]));		
	}
	
	printf("--------> CHEIO <--------- \n");
	
	for(i = 0; i < 4; i++) {
		printf("%d: %s \n", i, print_transaction(waiting_list[i]));
	}
	
	printf("--------> FIM <--------- \n");
	
	
	for(;;) {
		transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
		ponteiro = waiting_algorithm();
		printf("ponteiro: %d \n", ponteiro);
		printf("%d: %s \n", ponteiro, print_transaction(waiting_list[ponteiro]));
		fread(transaction, sizeof(transaction_t), 1, fifos[ponteiro]);
		waiting_list[ponteiro] = transaction;
		sleep(1);	
	}
	
}


int main(int argc, char* argv[]) {
	int i;
	pthread_t threads[NUMBER_OF_THREADS];
	readOrders();

	/*
	for(i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_create(&threads[i], NULL, readOrders, NULL);
	}

    for(i = 0; i < NUMBER_OF_THREADS; i++) {
		pthread_join(threads[i], NULL);
    }
    */
    
}
