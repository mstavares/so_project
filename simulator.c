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
#include "simulator.h"

#define NUMBER_OF_PIPES 4

static int fds[NUMBER_OF_PIPES];

/**
 * Esta funcao abre os fifos para leitura.
 */
void open_fifos(FILE *fifos[]) {
	for (int i = 0; i < NUMBER_OF_PIPES; i++) {
		char fifo[8];
		snprintf(fifo, sizeof(fifo), "fifo%d", i);
		fds[i] = open(fifo, O_RDONLY);
		printf("A abrir o %s \n", fifo);
	}
}

/**
 *
 */
fd_set create_pool() {
	fd_set set;
	FD_ZERO(&set);
	for(int i = 0; i < NUMBER_OF_PIPES; i++) {
		FD_SET(fds[i], &set);
	}
	return set;
}

/**
 * Esta funcao vai ler os fifos e encaminhar a ordem a ordem com maior precedencia.
 */
void read_orders(FILE *fifos[]) {
	for(;;) {
		sleep(1);
		fd_set set = create_pool();
		if(select(FD_SETSIZE, &set, NULL, NULL, NULL) != -1) {
			for(int i = 0; i < NUMBER_OF_PIPES; i++) {
				if(FD_ISSET(fds[i], &set)) {
					transaction_t* transaction = (transaction_t *) malloc(sizeof(transaction_t));
					read(fds[i], transaction, sizeof(transaction_t));
					printf("%d: %s \n", fds[i], print_transaction(transaction));
				}
			}
		} else {
			perror("select = -1");
			exit(EXIT_FAILURE);
		}
	}
}


/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	FILE *fifos[NUMBER_OF_PIPES];
	open_fifos(fifos);	
	read_orders(fifos);
}
