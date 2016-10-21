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

#define FIFO_PERMISSIONS 0666
#define PROCESSING_PIPES 10
#define PROCESSING_THREADS PROCESSING_PIPES
#define NUMBER_OF_PIPES 4
#define ORDERS 100
#define INIT_THREADS 12
#define PIPE_FDS 2

int pipe_queue[PIPE_FDS];
int pipe_Altri[PIPE_FDS];
int pipe_BPI[PIPE_FDS];
int pipe_Banif[PIPE_FDS];
int pipe_BCP[PIPE_FDS];
int pipe_CTT[PIPE_FDS];
int pipe_EDP[PIPE_FDS];
int pipe_Galp[PIPE_FDS];
int pipe_NOS[PIPE_FDS];
int pipe_PT[PIPE_FDS];
int pipe_REN[PIPE_FDS];

transaction_t Altri[ORDERS];
transaction_t BPI[ORDERS];
transaction_t Banif[ORDERS];
transaction_t BCP[ORDERS];
transaction_t CTT[ORDERS];
transaction_t EDP[ORDERS];
transaction_t Galp[ORDERS];
transaction_t NOS[ORDERS];
transaction_t PT[ORDERS];
transaction_t REN[ORDERS];

int *processing_pipes[PROCESSING_PIPES] = {pipe_Altri, pipe_BPI,
		pipe_Banif, pipe_BCP, pipe_CTT, pipe_EDP, pipe_Galp, pipe_NOS, pipe_PT,
		pipe_REN};

transaction_t *transactions[PROCESSING_PIPES] = {Altri, BPI, Banif, BCP, CTT, 
		EDP, Galp, NOS, PT, REN};
		
sem_t *semaphores[PROCESSING_PIPES];

sem_t sem;




int get_free_position(transaction_t array[]) {
	int i;
	for (i = 0; i < ORDERS; i++) {
		if (array[i].value == 0) {
			break;
		}
	}
	return i;
}

void* process_orders(void *received) {
	int position =  *((int *) received);
	printf("position: %d \n", position);
	transaction_t *array = transactions[PROCESSING_PIPES];
	for(;;) {
		//sem_wait(semaphores[position]);
		transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
		read(processing_pipes[position][0], transaction, sizeof(transaction_t));
		printf("!!!! %s \n", print_transaction(transaction));
		
		/*
		
		int index = get_free_position(array);
		for (int i = 0; i < ORDERS; i++) {
			if (transaction->value > 0) {
				if ((array[i].value < 0) && (fabsf(array[i].value) <= transaction->value)) {
					if (transaction->amount > array[i].amount) {
						printf("Transacionadas %d ações de array por %.2f\n", array[i].amount, fabsf(array[i].value));
						//fprintf(fp, "Transacionadas %d ações de array por %.2f\n", array[i].amount, fabsf(array[i].value));
						//fclose(fp);
						transaction->amount -= array[i].amount;
						array[i].amount = 0;
						memcpy(&array[index], transaction, sizeof(transaction_t));
						if (array[index].amount == 0) 
							memset(&array[index], 0, sizeof(transaction_t));
						if (array[i].amount == 0) 
							memset(&array[i], 0, sizeof(transaction_t));
					} else {
						printf("Transacionadas %d ações de array por %.2f\n", array[i].amount, fabsf(array[i].value));
						//fprintf(fp, "Transacionadas %d ações de array por %.2f\n", transaction->amount, fabsf(array[i].value));
						//fclose(fp);
						array[i].amount -= transaction->amount;
						transaction->amount = 0;
						memcpy(&array[index], transaction, sizeof(transaction_t));
						if (array[index].amount == 0) memset(&array[index], 0, sizeof(transaction_t));
						if (array[i].amount == 0) memset(&array[i], 0, sizeof(transaction_t));
					}
				} else {
					memcpy(&array[index], transaction, sizeof(transaction_t));
				}
			}
			if (transaction->value < 0) {
				if ((array[i].value > 0) && (array[i].value >= fabsf(transaction->value))) {
					if (transaction->amount > array[i].amount) {
						transaction->amount -= array[i].amount;
						printf("Transacionadas %d ações de array por %.2f\n", array[i].amount, fabsf(array[i].value));
						//fprintf(fp, "Transacionadas %d ações de array por %.2f\n", array[i].amount, fabsf(array[i].value));
						//fclose(fp);
						array[i].amount = 0;
						memcpy(&array[index], transaction, sizeof(transaction_t));
						if (array[index].amount == 0) memset(&array[index], 0, sizeof(transaction_t));
						if (array[i].amount == 0) memset(&array[i], 0, sizeof(transaction_t));
					}
					else {
						array[i].amount -= transaction->amount;
						printf("Transacionadas %d ações de array por %.2f\n", array[i].amount, fabsf(array[i].value));
						//fprintf(fp, "Transacionadas %d ações de array por %.2f\n", transaction->amount, fabsf(array[i].value));
						//fclose(fp);
						transaction->amount = 0;
						memcpy(&array[index], transaction, sizeof(transaction_t));
						if (array[index].amount == 0) memset(&array[index], 0, sizeof(transaction_t));
						if (array[i].amount == 0) memset(&array[i], 0, sizeof(transaction_t));
					}
				} else {
					memcpy(&array[index], transaction, sizeof(transaction_t));
				}
			}
		}
		*/
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

void create_pipes() {
	for(int i = 0; i < PROCESSING_PIPES; i++) {
		if(pipe(processing_pipes[i])) {
			perror("Erro ao criar o pipe. \n");
		}
	}
	if(pipe(pipe_queue)) {
		perror("Erro ao criar o pipe. \n");
	}
}

void init_semaphores() {
	sem_init(&sem, 0, 1);
	for(int i = 0; i < PROCESSING_PIPES; i++) {
		sem_init(semaphores[i], 0, 1);
	}
}

void setup() {
	FILE *fifos[NUMBER_OF_PIPES];
	
	open_fifos(fifos);
	create_pipes();
}


void allocate_orders(transaction_t *transaction) {
	if(strcmp(transaction->title, titles[0]) == 0) {
		write(pipe_Altri[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[0]);
	} else if (strcmp(transaction->title, titles[1]) == 0) {
		write(pipe_BPI[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[1]);
	} else if (strcmp(transaction->title, titles[2]) == 0) {
		write(pipe_Banif[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[2]);
	} else if (strcmp(transaction->title, titles[3]) == 0) {
		write(pipe_BCP[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[3]);
	} else if (strcmp(transaction->title, titles[4]) == 0) {
		write(pipe_CTT[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[4]);
	} else if (strcmp(transaction->title, titles[5]) == 0) {
		write(pipe_EDP[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[5]);
	} else if (strcmp(transaction->title, titles[6]) == 0) {
		write(pipe_Galp[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[6]);
	} else if (strcmp(transaction->title, titles[7]) == 0) {
		write(pipe_NOS[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[7]);
	} else if (strcmp(transaction->title, titles[8]) == 0) {
		write(pipe_PT[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[8]);
	} else if (strcmp(transaction->title, titles[9]) == 0) {
		write(pipe_REN[1], transaction, sizeof(transaction_t));
		//sem_post(semaphores[9]);
	}
}

void* main_thread() {
	for(;;) {
		sem_wait(&sem);
		sleep(1);
		transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
		read(pipe_queue[0], transaction, sizeof(transaction_t));
		allocate_orders(transaction);
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
		transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
		read(fd, transaction, sizeof(transaction_t));
		write(pipe_queue[1], transaction, sizeof(transaction_t));
		free(transaction);
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

/**
 *
 */
void start() {
	int *taskids[NUMBER_OF_PIPES];
	*taskids[0] = 0;
	
	int numero = 0;
	
	pthread_t threads[12];
	
    pthread_create(&threads[0], NULL, start_reading_threads, NULL);
    pthread_create(&threads[1], NULL, start_main_thread, NULL);
    pthread_create(&threads[3], NULL, process_orders, (void *) taskids[0]);
        
    /*
    for(int i = 0; i < 10; i++){
    	taskids[i] = (int *) malloc(sizeof(int));
    	*taskids[i] = i;
    	pthread_create(&threads[i + 2], NULL, process_orders, (void *) taskids[i]);
    }
    */
    
    
    for(int i = 0; i < 3; i++) {
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
