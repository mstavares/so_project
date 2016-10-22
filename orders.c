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
#define FILE_NAME "orders.txt"

static sem_t *semaphores[NUMBER_OF_PIPES];


FILE * is_file_exists() {
	return fopen(FILE_NAME, "r");
}

void read_file(FILE *file) {
	int c;
	while ((c = getc(file)) != EOF)
    	putchar(c);
	fclose(file);
}

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
 * Esta funcao le as ordens do ficheiro FILE_NAME e envia as pelo fifo0
 */
void read_orders_from_file(FILE *file, char* ficheiro, int fifo_number) {
	printf("A ler ordens do ficheiro orders.txt \n");
	while(!feof(file)) {
		transaction_t *transaction = transaction_from_file(file);
		if(transaction != NULL) {
			write(open(ficheiro, O_WRONLY), transaction, sizeof(transaction_t));
			printf("%d -> %s", fifo_number, print_transaction(transaction));
			sem_post(semaphores[fifo_number]);
		}
	}
	printf("A leitura do ficheiro orders.txt foi terminada \n");
}

/**
 * Esta é a função invocada pelas threads e é a responsável por gerar
 * as ordens aleatóriamente. Estas ordens são posteriormente inseridas
 * nos fifos.
 */
void* generate_orders(void* i) {
	FILE *file;
	int fifo_number = *((int*) i);
	char ficheiro[8];
	sprintf(ficheiro, "fifo%d", fifo_number);
	if(fifo_number == 0 && (file = is_file_exists())) {
		read_orders_from_file(file, ficheiro, fifo_number);
	} else {
		printf("A gerar ordens random no %s \n", ficheiro);
		for(;;) {
			sleep(rand() % 10);
			transaction_t *transaction = create_transaction();
			write(open(ficheiro, O_WRONLY), transaction, sizeof(transaction_t));
			printf("%d -> %s", fifo_number, print_transaction(transaction));
			sem_post(semaphores[fifo_number]);
		}
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
