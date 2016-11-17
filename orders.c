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

//#define NUMBER_OF_PIPES 5
#define STDIN_THREAD NUMBER_OF_PIPES - 1
#define FIFO_PERMISSIONS 0666

/** Este array de semaforos e necessario para a comunicacao com o simulator.c */
static sem_t *semaphores[NUMBER_OF_PIPES];

/** */
char *file_names[4] = {"orders0.txt", "orders1.txt", "orders2.txt", "orders3.txt"};

int orders_to_generate = 0;

int number_of_orders_per_second = 0;

/**
 * Esta funcao verifica se um determinado ficheiro existe
 * se sim, retorna um hander para o mesmo.
 */
FILE * is_file_exists(char* file_name) {
	return fopen(file_name, "r");
}


/**
 * Esta funcao le as ordens do ficheiro FILE_NAME_TO_READ e envia as pelo fifo0
 */
void read_orders_from_file(FILE *file, char* ficheiro, int fifo_number) {
	int fd = open(ficheiro, O_WRONLY);
	printf("A ler ordens do ficheiro orders[%d].txt \n", fifo_number);
	while(!feof(file)) {
		transaction_t *transaction = transaction_from_file(file);
		if(transaction != NULL) {
			write(fd, transaction, sizeof(transaction_t));
			sem_post(semaphores[fifo_number]);
		}
	}
	printf("A leitura do ficheiro orders[%d].txt foi terminada \n", fifo_number);
}

/**
 * Esta funcao escreve as ordens num determinado fifo e aciona o semaforo
 * respeitante ao fifo em questão
 */
void write_orders(int fd, sem_t *sem, transaction_t * (*fp_transaction)()) {
	for(;;) {
		sleep(1);
		transaction_t *transaction = (*fp_transaction)();
		write(fd, transaction, sizeof(transaction_t));
		sem_post(sem);
	}
}

/**
 * Esta funcao vai verificar se:
 * 1: Temos ordens para ler a partir de ficheiros
 * 2: Não temos ficheiros vai gerar ordens aleatorias
 * 3: Temos de criar a thread para o utilizador criar ordens
 */
void * thread_dispatcher(void* i) {
	FILE *file;
	int fifo_number = *((int*) i);
	char ficheiro[8];
	sprintf(ficheiro, "fifo%d", fifo_number);
	int fd = open(ficheiro, O_WRONLY);
	if(fifo_number == 4) {
		write_orders(fd, semaphores[fifo_number], transaction_manual_create);
	} else {
		if(file = is_file_exists(file_names[fifo_number])) {
			read_orders_from_file(file, ficheiro, fifo_number);
		} else {
			for(;;) {
				write_orders(fd, semaphores[fifo_number], transaction_create);
			}
		}
	}
}


/**
 * Esta função inicializa as threads que vao gerar ordens
 * aleatórias para dentro dos fifos
 */
void start_threads() {
	int *taskids[4];
	pthread_t threads[4];
	
    for(int i = 0; i < 4; i++) {
    	taskids[i] = (int *) malloc(sizeof(int));
    	*taskids[i] = i;
    	pthread_create(&threads[i], NULL, thread_dispatcher, (void *) taskids[i]);
	}
	  
    for(int i = 0; i < 4; i++) {
		pthread_join(threads[i], NULL);
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
 		unlink(semaphore);	
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
