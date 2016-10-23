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
#include <sys/mman.h>
#include "transaction.h"
#include "simulator.h"
#include "queue.h"

#define FIFO_PERMISSIONS 0666
#define NUMBER_OF_PROCESSING_THREADS 10
#define MAIN_THREADS_TO_START 3
#define NUMBER_OF_PIPES 4
#define ORDERS 200
#define INIT_THREADS 12

transaction_t queue[ORDERS];
transaction_t Altri[ORDERS];
transaction_t BPI[ORDERS];
transaction_t Banif[ORDERS];
transaction_t BCP[ORDERS];
transaction_t CTT[ORDERS];
transaction_t EDP[ORDERS];
transaction_t Galp[ORDERS];
transaction_t NOS[ORDERS];
transaction_t Pharol[ORDERS];
transaction_t REN[ORDERS];

transaction_t *transactions[NUMBER_OF_PROCESSING_THREADS] = {Altri, BPI, Banif, BCP, CTT, 
		EDP, Galp, NOS, Pharol, REN};
		
sem_t processing_semaphores[NUMBER_OF_PROCESSING_THREADS];

static sem_t *semaphores[NUMBER_OF_PIPES];

sem_t sem;


void* process_orders(void *received) {
	int position =  *((int *) received);
	transaction_t *array = transactions[position];
	for(;;) {
		sem_wait(&processing_semaphores[position]);
		printf("---------------------\n");
		queue_sort(array, ORDERS);
		queue_print(array, ORDERS);
		printf("**********************\n");
		for(int i = 0; i < ORDERS; i++) {
			if((array + i)->value > 0) {
				/** Vai comprar ações */
				for(int j = 0; j < ORDERS; j++) {
					if((array + j)->value < 0 && (array + i)->value >= fabs((array + j)->value)) {
						if((array + i)->amount > (array + j)->amount) {
							/** Comprou as acções todas */
							printf("1: Transacionadas %d ações de %s por %.2f\n", (array + j)->amount, 
								(array + j)->title ,fabsf((array + j)->value));
								
							printf("%s -> %s \n", (array + j)->id, (array + i)->id);

							(array + i)->amount -= (array + j)->amount;
							(array + j)->amount = 0;							
							memset(array + j, 0, sizeof(transaction_t));
						} else {
							/** As ações nao foram totalmente compradas */
							printf("2: Transacionadas %d ações de %s por %.2f\n", (array + i)->amount, 
								(array + i)->title ,fabsf((array + i)->value));
								
							printf("%s -> %s \n", (array + j)->id, (array + i)->id);
							
							(array + j)->amount -= (array + i)->amount;
							(array + i)->amount = 0;
							memset(array + i, 0, sizeof(transaction_t));
						}	
					}
				}
			}
		}
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
 * Esta funcao abre os fifos para leitura.
 */
void open_fifos(FILE *fifos[]) {
	for (int i = 0; i < NUMBER_OF_PIPES; i++) {
		char fifo[8];
		snprintf(fifo, sizeof(fifo), "fifo%d", i);
	}
}


void init_semaphores() {
	sem_init(&sem, 0, 0);
	for(int i = 0; i < NUMBER_OF_PROCESSING_THREADS; i++) {
		sem_init(&processing_semaphores[i], 0, 0);
	}
	

}


void setup() {
	FILE *fifos[NUMBER_OF_PIPES];
	open_fifos(fifos);
	init_semaphores();
	create_semaphores();
}


void allocate_orders(transaction_t *transaction) {
	if(strcmp(transaction->title, titles[0]) == 0) {
		queue_push(Altri, ORDERS, transaction);
		sem_post(&processing_semaphores[0]);
	} /*else if (strcmp(transaction->title, titles[1]) == 0) {
		queue_push(BPI, ORDERS, transaction);
		sem_post(&processing_semaphores[1]);
	} else if (strcmp(transaction->title, titles[2]) == 0) {
		queue_push(Banif, ORDERS, transaction);
		sem_post(&processing_semaphores[2]);
	} else if (strcmp(transaction->title, titles[3]) == 0) {
		queue_push(BCP, ORDERS, transaction);
		sem_post(&processing_semaphores[3]);
	} else if (strcmp(transaction->title, titles[4]) == 0) {
		queue_push(CTT, ORDERS, transaction);
		sem_post(&processing_semaphores[4]);
	} else if (strcmp(transaction->title, titles[5]) == 0) {
		queue_push(EDP, ORDERS, transaction);
		sem_post(&processing_semaphores[5]);
	} else if (strcmp(transaction->title, titles[6]) == 0) {
		queue_push(Galp, ORDERS, transaction);
		sem_post(&processing_semaphores[6]);
	} else if (strcmp(transaction->title, titles[7]) == 0) {
		queue_push(NOS, ORDERS, transaction);
		sem_post(&processing_semaphores[7]);
	} else if (strcmp(transaction->title, titles[8]) == 0) {
		queue_push(Pharol, ORDERS, transaction);
		sem_post(&processing_semaphores[8]);
	} else if (strcmp(transaction->title, titles[9]) == 0) {
		queue_push(REN, ORDERS, transaction);
		sem_post(&processing_semaphores[9]);
	}
	*/
}


void* main_thread() {
	for(;;) {
		sem_wait(&sem);
		sleep(1);
		allocate_orders(queue_pop(queue, ORDERS));
	}
}


void* start_main_thread() {
	pthread_t thread;
	pthread_create(&thread, NULL, main_thread, NULL);   
	pthread_join(thread, NULL);
}


/**
 * Esta funcao vai ler os fifos e encaminhar a ordem a ordem com maior precedencia.
 */
void* read_orders(void* i) {
	char ficheiro[8];
	int pipe_number = *((int *) i);
	sprintf(ficheiro, "fifo%d", pipe_number);
	printf("A ler ordens do %s \n", ficheiro);
	int fd = open(ficheiro, O_RDONLY);		
	for(;;) {
		sem_wait(semaphores[pipe_number]);
		transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
		read(fd, transaction, sizeof(transaction_t));
		queue_push(queue, ORDERS, transaction);
		sem_post(&sem);
	}	
}


/**
 * Esta função inicializa as threads que vao ler as ordens
 * contidas nos fifos
 */
void* start_reading_threads() {
	int *taskids[NUMBER_OF_PIPES];
	pthread_t threads[NUMBER_OF_PIPES];
	
    for(int i = 0; i < NUMBER_OF_PIPES; i++) {
    	taskids[i] = (int *) malloc(sizeof(int));
    	*taskids[i] = i;
		pthread_create(&threads[i], NULL, read_orders, (void *) taskids[i]);
    }  
  
    for(int i = 0; i < NUMBER_OF_PIPES; i++) {
		pthread_join(threads[i], NULL);
    }  
}


void* start_processing_threads() {
	int *taskids[NUMBER_OF_PROCESSING_THREADS];
	pthread_t threads[NUMBER_OF_PROCESSING_THREADS];
	
    for(int i = 0; i < NUMBER_OF_PROCESSING_THREADS; i++) {
    	taskids[i] = (int *) malloc(sizeof(int));
    	*taskids[i] = i;
		pthread_create(&threads[i], NULL, process_orders, (void *) taskids[i]);
    }  
  
    for(int i = 0; i < NUMBER_OF_PROCESSING_THREADS; i++) {
		pthread_join(threads[i], NULL);
    } 
}


/**
 *
 */
void start() {
	pthread_t threads[MAIN_THREADS_TO_START];
	
    pthread_create(&threads[0], NULL, start_reading_threads, NULL);
    pthread_create(&threads[1], NULL, start_main_thread, NULL);
    pthread_create(&threads[2], NULL, start_processing_threads, NULL);
   
    for(int i = 0; i < MAIN_THREADS_TO_START; i++) {
		pthread_join(threads[i], NULL);
    }    
}


/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	setup();
	start();
}
