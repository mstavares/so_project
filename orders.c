#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <unistd.h>
#include "transaction.h"

#define NUM_THREADS 4
#define NUMBER_OF_PIPES 4

sem_t *semaphores[NUMBER_OF_PIPES];

void* generate_orders(void* i);



void create_semaphores() {
	char semaphore[10];
	for(int i = 0; i < NUMBER_OF_PIPES; i++) {
		snprintf(semaphore, sizeof(semaphore), "semafore%d", i);
		semaphores[i] = sem_open(semaphore, O_CREAT, 00700, 0);  
	}
}


void create_pipes() {
	int i, ret_val;
	char fifo[8];
	for (i = 0; i < NUMBER_OF_PIPES; i++) {
		snprintf(fifo, sizeof(fifo), "fifo%d", i);	
		ret_val = mkfifo(fifo, 0666);
		if ((ret_val == -1) && (errno != EEXIST)) {
			perror("Error creating named pipe\n");
			exit(-1);
		}
	}
	
}

void start_threads() {
	int *taskids[NUM_THREADS];
	pthread_t threads[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
    	taskids[i] = (int *) malloc(sizeof(int));
    	*taskids[i] = i;
		pthread_create(&threads[i], NULL, generate_orders, (void *) taskids[i]);
    }   
    
    for(int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
    }  
}


void* generate_orders(void* i) {
	char ficheiro[8];
	int pipe_number = *((int*) i);
	sprintf(ficheiro, "fifo%d", pipe_number);
	printf("a gerar ordens random no %s \n", ficheiro);
	//FILE *file_fifo = fopen(ficheiro, "wb");
	int wrfd = open(ficheiro, O_WRONLY);
	
	int val;
	
	for(;;) {
		sleep(rand() % 10);
		
		transaction_t *transaction = create_transaction();
		write(wrfd, transaction, sizeof(transaction_t));
		printf("%d -> %s \n", pipe_number, print_transaction(transaction));
		free(transaction);
		
		//sem_getvalue(semaphores[pipe_number], &val);
		//printf(":%d \n", val);
	//	sem_wait(semaphores[pipe_number]);
		//sem_post(semaphores[pipe_number]);
	}	
}

void teste() {
	for(;;) {
		sem_wait(semaphores[0]);
	}
}


/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	create_pipes();
	create_semaphores();
	start_threads();
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
}
