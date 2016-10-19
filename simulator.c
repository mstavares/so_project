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

#define NUMBER_OF_PIPES 4

sem_t *semaphores[NUMBER_OF_PIPES];
static transaction_t* waiting_list[NUMBER_OF_PIPES];
static int ponteiro;

void teste();

/**
 * Esta função define que ordem é que passa primeiro.
 */
 
int compare(time_t a, time_t b) {
	if(a <= b) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * Este algoritmo devolve a posicao da próxima ordem a sair da lista de espera.
 */
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

/**
 * Esta funcao abre os fifos para leitura.
 */
void open_fifos(FILE *fifos[]) {
	for (int i = 0; i < NUMBER_OF_PIPES; i++) {
		char fifo[8];
		snprintf(fifo, sizeof(fifo), "fifo%d", i);
		fifos[i] = fopen(fifo, "rb");
		printf("A abrir o %s \n", fifo);
	}
}

/**
 * Esta funcao vai carregar a lista de espera com as primeiras ordens de cada pipe
 */
void buffering(FILE *fifos[]) {
	
	for(int i = 0; i < NUMBER_OF_PIPES; i++) {
		sleep(1);
		waiting_list[i] = (transaction_t *) malloc(sizeof(transaction_t));
		fread(waiting_list[i], sizeof(transaction_t), 1, fifos[i]);
		//sem_wait(semaphores[i]);
	}
}

/**
 * Esta funcao vai ler os fifos e encaminhar a ordem a ordem com maior precedencia.
 */
void read_orders(FILE *fifos[]) {
	for(;;) {
		ponteiro = waiting_algorithm();
		sleep(1);
		printf("%d: %s \n", ponteiro, print_transaction(waiting_list[ponteiro]));
		free(waiting_list[ponteiro]);
		waiting_list[ponteiro] = (transaction_t *) malloc(sizeof(transaction_t));
		fread(waiting_list[ponteiro], sizeof(transaction_t), 1, fifos[ponteiro]);
		//sem_wait(semaphores[ponteiro]);
	}
}


void create_semaphores() {
	char semaphore[10];
	for(int i = 0; i < NUMBER_OF_PIPES; i++) {
		snprintf(semaphore, sizeof(semaphore), "semafore%d", i);
		semaphores[i] = sem_open(semaphore, O_CREAT, 00700, 0);  
	}
}


/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	FILE *fifos[NUMBER_OF_PIPES];
	open_fifos(fifos);
	create_semaphores();	
	buffering(fifos);
	read_orders(fifos);
}
