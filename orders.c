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
#define NUM_PIPES 4

pthread_mutex_t lock;

void create_pipes() {
	int i, ret_val;
	char fifo[8];
	for (i = 0; i < NUM_PIPES; i++) {
		snprintf(fifo, sizeof(fifo), "fifo%d", i);	
		ret_val = mkfifo(fifo, 0666);
		if ((ret_val == -1) && (errno != EEXIST)) {
			perror("Error creating named pipe\n");
			exit(-1);
		}
	}
	
}


void* teste(void* fifo) {
	int i, fd;
	char* ficheiro = (char*) fifo;
	printf("imprime: %s \n", ficheiro);
	FILE *f = fopen(ficheiro, "wb");
	for(;;) {
		transaction_t *transaction = create_transaction();
		fwrite(transaction, sizeof(transaction_t), 1, f);
	}
	fclose(f);	
}

/*
void* teste2(void* fifo) {
	int i;
	char *ficheiro = (char*) fifo;
	printf("recebe: %s \n", ficheiro);
	
	FILE *r = fopen(ficheiro, "rb");
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	for(;;) {
		sleep(1);
		fread(transaction, sizeof(transaction_t), 1, r);
		printf("%s: %s \n",ficheiro, print_transaction(transaction));
	}
	fclose(r);
}


/**
 *
 */
//void start(int number_of_orders) {
void start() {
	create_pipes();

	pthread_t threads[NUM_THREADS];
	int rc, t;
    int *taskids[NUM_THREADS];
    int i;

    char* fifos[4] = {"fifo0", "fifo1", "fifo2", "fifo3"};
    
    for(i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, teste, (void *) fifos[i]);
    }   
    
    for(i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
    }
    
    
}

/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	start();
}
