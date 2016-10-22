#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <unistd.h>
#include "transaction.h"
#include "orders.h"

#define NUMBER_OF_PIPES 4
#define FIFO_PERMISSIONS 0666

static sem_t *semaphores[NUMBER_OF_PIPES];

void create_semaphores() {		
 	char semaphore[10];		
 	for(int i = 0; i < NUMBER_OF_PIPES; i++) {		
 		snprintf(semaphore, sizeof(semaphore), "semafore%d", i);		
 		semaphores[i] = sem_open(semaphore, O_CREAT, 00700, 0);  		
 	}		
}

/**
 * Esta função cria os fifos para que haja comunicacao com o simulator.c
 */
void create_fifos() {
	char fifo[8];
	for (int i = 0; i < NUMBER_OF_PIPES; i++) {
		snprintf(fifo, sizeof(fifo), "fifo%d", i);	
		if ((mkfifo(fifo, FIFO_PERMISSIONS) == -1) && (errno != EEXIST)) {
			perror("Error creating named pipe\n");
			exit(EXIT_FAILURE);
		}
	}
}

/**
 * Esta função inicializa as threads que vao gerar ordens
 * aleatórias para dentro dos fifos
 */
void start_threads() {
	int *taskids[NUMBER_OF_PIPES];
	pthread_t threads[NUMBER_OF_PIPES];
	
    for(int i = 0; i < NUMBER_OF_PIPES; i++) {
    	taskids[i] = (int *) malloc(sizeof(int));
    	*taskids[i] = i;
		pthread_create(&threads[i], NULL, generate_orders, (void *) taskids[i]);
    }   
  
    for(int i = 0; i < NUMBER_OF_PIPES; i++) {
		pthread_join(threads[i], NULL);
    }  
}

/**
 * Esta é a função invocada pelas threads e é a responsável por gerar
 * as ordens aleatóriamente. Estas ordens são posteriormente inseridas
 * nos fifos.
 */
void* generate_orders(void* i) {
	char ficheiro[8];
	int pipe_number = *((int*) i);
	sprintf(ficheiro, "fifo%d", pipe_number);
	printf("A gerar ordens random no %s \n", ficheiro);
	int fd = open(ficheiro, O_WRONLY);
	for(;;) {
		sleep(rand() % 10);
		transaction_t *transaction = create_transaction();
		write(fd, transaction, sizeof(transaction_t));
		printf("%d -> %s \n", pipe_number, print_transaction(transaction));
		free(transaction);
		sem_post(semaphores[pipe_number]);
		
		/*
		printf("antes pipe: %d \n", pipe_number);
		int val;
		sem_getvalue(semaphores[pipe_number], &val);
		printf("valor: %d \n", val);
		
		sem_post(semaphores[pipe_number]);
		
		printf("depois pipe: %d \n", pipe_number);;
		sem_getvalue(semaphores[pipe_number], &val);
		printf("valor: %d \n", val);
		*/
	}	
}

void setup() {
	create_fifos();
	create_semaphores();
}

/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	setup();
	start_threads();	
}
