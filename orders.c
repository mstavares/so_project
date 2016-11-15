/**********************************************************************
 * FICHEIRO: orders.c
 * DESCRICAO:
 *   Este programa cria ordens aleatorias, le ordens atraves de ficheiros
 *   e atraves do teclado.
 *   Estas ordens sao escritas em cinco pipes
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

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
#define FILE_NAME_TO_READ "orders.txt"

/** Este array de semaforos e necessario para a comunicacao com o simulator.c */
static sem_t *semaphores[NUMBER_OF_PIPES];

/**
 * Esta funcao verifica se um determinado ficheiro existe
 * se sim, retorna um hander para o mesmo.
 */
FILE * is_file_exists(char* file_name) {
	return fopen(file_name, "r");
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
 * Esta funcao le as ordens do ficheiro FILE_NAME_TO_READ e envia as pelo fifo0
 */
void read_orders_from_file(FILE *file, char* ficheiro, int fifo_number) {
	printf("A ler ordens do ficheiro orders.txt \n");
	while(!feof(file)) {
		transaction_t *transaction = transaction_from_file(file);
		if(transaction != NULL) {
			write(open(ficheiro, O_WRONLY), transaction, sizeof(transaction_t));
			printf("%d -> %s", fifo_number, transaction_print(transaction));
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
void * generate_orders(void* i) {
	FILE *file;
	int fifo_number = *((int*) i);
	char ficheiro[8];
	sprintf(ficheiro, "fifo%d", fifo_number);
	if(fifo_number == 0 && (file = is_file_exists(FILE_NAME_TO_READ))) {
		read_orders_from_file(file, ficheiro, fifo_number);
	} else {
		printf("A gerar ordens random no %s \n", ficheiro);
		for(;;) {
			sleep(rand() % 10);
			transaction_t *transaction = transaction_create();
			write(open(ficheiro, O_WRONLY), transaction, sizeof(transaction_t));
			printf("%d -> %s", fifo_number, transaction_print(transaction));
			sem_post(semaphores[fifo_number]);
		}
	}	
}


/**
 * Esta funcao cria os semaforos utilizados na comunicacao com
 * o simulator.c
 */
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
 * Esta faz a inicializacao do orders.c
 */
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
