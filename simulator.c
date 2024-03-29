/**********************************************************************
 * FICHEIRO: simulator.c
 * DESCRICAO:
 *   Este programa le ordens através de cinco fifos e aloca as em
 *	 repositorios para processamento.
 *   Este processamento são as compras e as vendas das acoes
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

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
#include <signal.h>
#include <sys/mman.h>
#include "transaction.h"
#include "simulator.h"
#include "orders.h"
#include "queue.h"

#define NUMBER_OF_PROCESSING_THREADS 10
#define MAIN_THREADS_TO_START 3
#define INIT_THREADS 12

/** Estes arrays são os repositorios das ordens */
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

/** Array de repositorios para tornar o codigo a funcao process_orders generica */
transaction_t *transactions[NUMBER_OF_PROCESSING_THREADS] = {Altri, BPI, Banif, BCP, CTT, 
		EDP, Galp, NOS, Pharol, REN};

/** Estes 10 semaforos dizem respeito aos repositorios de ordens */	
sem_t processing_semaphores[NUMBER_OF_PROCESSING_THREADS];

/** Estes sao os 5 semaforos que sao acionados pelo orders.c */
static sem_t *semaphores[NUMBER_OF_PIPES];

/** Este semaforo e utilizado para a memoria partilhada */
static sem_t *shared_memory_sem;

/** 
 * Este semaforo e acionado pelas 5 threads que estao a ler dos fifos
 * A dispatcher thread tambem usa este semaforo
 */
sem_t fifos_sem;

/** Este mutex gere o acesso ao ficheiro de logs */
pthread_mutex_t lock;

/** Memoria partilhada */
transaction_t *shared_memory;

/** Tamanho da memoria partilhada */
long size_of_shared_memory = sizeof(transaction_t) * ORDERS;

/** Este mutex gere o acesso à principal queue de orders */
pthread_mutex_t queue_lock;


/**
 * Funcao que escreve no ficheiro de logs
 */
void write_to_file(char* string_to_write, char* file_name) {
	pthread_mutex_lock(&lock);
	char path[50] = "output/";
	strcat(path, file_name);
	FILE * file = fopen(path, "a");
	fprintf(file, "%s", string_to_write);
	fclose(file);
	pthread_mutex_unlock(&lock);
}


/**
 * Thread que processa os repositorios e escreve no ficheiro de logs
 */
void * process_orders(void *received) {
	char string[50];
	int position =  *((int *) received);
	transaction_t *array = transactions[position];
	char* transactions_file_name = "simulador.log";
	for(;;) {
		sem_wait(&processing_semaphores[position]);	
		for(int i = 0; i < ORDERS; i++) {
			if(fabs((array + i)->value == 0)) {
				break;
			} else {
				for(int j = 0; j < ORDERS; j++) {
					if(fabs((array + j)->value == 0)) {
						break;
					} else {
						if((array + j)->value < 0 && (array + i)->value >= fabs((array + j)->value)) {
							if((array + i)->amount > (array + j)->amount) {
								/** Todas as acções foram transacionadas */
								printf("%s transacionou %d ações de %s por %.2f em %s\n", (array + j)->id, 
								abs((array + j)->amount), (array + j)->title ,fabsf((array + j)->value), (array + j)->timestamp);
								/** Escreve o o registo da transacao no ficheiro simulador.log */
								sprintf(string, "%s transacionou %d ações de %s por %.2f em %s\n", (array + j)->id, 
								(array + j)->amount, (array + j)->title, fabs((array + j)->value), (array + j)->timestamp);	 
								write_to_file(string, transactions_file_name);
								(array + i)->amount -= (array + j)->amount;						
								memset(array + j, 0, sizeof(transaction_t));
							} else {
								/** As ações nao foram totalmente transacionadas */
								printf("%s transacionou %d ações de %s por %.2f em %s\n", (array + i)->id, 
								abs((array + i)->amount), (array + j)->title ,fabsf((array + j)->value), (array + i)->timestamp);	
								/** Escreve o o registo da transacao no ficheiro simulador.log */			
								sprintf(string, "%s transacionou %d ações de %s por %.2f em %s\n", (array + i)->id, 
								(array + i)->amount, (array + j)->title, fabs((array + j)->value), (array + i)->timestamp);	
								write_to_file(string, transactions_file_name);
								if((array + i)->amount == (array + j)->amount) {
									memset(array + i, 0, sizeof(transaction_t));
									memset(array + j, 0, sizeof(transaction_t));
								} else {						
									(array + j)->amount -= (array + i)->amount;
									memset(array + i, 0, sizeof(transaction_t));
								}		
							}
						}
					}
				}
			}
		}
	}
}


/**
 * Funcao que escreve na memoria partilhada
 */
void write_on_shared_memory(transaction_t array[]) {
	memcpy(shared_memory, array, size_of_shared_memory);
}


/**
 * Faz o trabalho da dispatcher thread
 */
void allocate_orders(transaction_t *transaction) {
	char *allocated_file_name = "alocadas.log";
	write_to_file(transaction_print(transaction), allocated_file_name);
	if(strcmp(transaction->title, titles[0]) == 0) {
		queue_push(Altri, ORDERS, transaction);
		sem_post(&processing_semaphores[0]);
		write_on_shared_memory(Altri);
	} else if (strcmp(transaction->title, titles[1]) == 0) {
		queue_push(BPI, ORDERS, transaction);
		sem_post(&processing_semaphores[1]);
		write_on_shared_memory(BPI);
	} else if (strcmp(transaction->title, titles[2]) == 0) {
		queue_push(Banif, ORDERS, transaction);
		sem_post(&processing_semaphores[2]);
		write_on_shared_memory(Banif);
	} else if (strcmp(transaction->title, titles[3]) == 0) {
		queue_push(BCP, ORDERS, transaction);
		sem_post(&processing_semaphores[3]);
		write_on_shared_memory(BCP);
	} else if (strcmp(transaction->title, titles[4]) == 0) {
		queue_push(CTT, ORDERS, transaction);
		sem_post(&processing_semaphores[4]);
		write_on_shared_memory(CTT);
	} else if (strcmp(transaction->title, titles[5]) == 0) {
		queue_push(EDP, ORDERS, transaction);
		sem_post(&processing_semaphores[5]);
		write_on_shared_memory(EDP);
	} else if (strcmp(transaction->title, titles[6]) == 0) {
		queue_push(Galp, ORDERS, transaction);
		sem_post(&processing_semaphores[6]);
		write_on_shared_memory(Galp);
	} else if (strcmp(transaction->title, titles[7]) == 0) {
		queue_push(NOS, ORDERS, transaction);
		sem_post(&processing_semaphores[7]);
		write_on_shared_memory(NOS);
	} else if (strcmp(transaction->title, titles[8]) == 0) {
		queue_push(Pharol, ORDERS, transaction);
		sem_post(&processing_semaphores[8]);
		write_on_shared_memory(Pharol);
	} else if (strcmp(transaction->title, titles[9]) == 0) {
		queue_push(REN, ORDERS, transaction);
		sem_post(&processing_semaphores[9]);
		write_on_shared_memory(REN);
	}
	sem_post(shared_memory_sem);
}


/**
 * A dispatcher thread é responsável por:
 * 1: Ler as ordens dos 5 fifos
 * 2: Entregar a ordem no repositorio de ordens correspondente
 * 3: Escrever na memoria partilhada
 * 4: Acionar o semaforo da memoria partilhada
 */
void * dispatcher_thread() {
	int i = 0;
	for(;;) {
		sleep(1);
		sem_wait(&fifos_sem);
		pthread_mutex_lock(&queue_lock);
		allocate_orders(queue_pop(queue, ORDERS));
		pthread_mutex_unlock(&queue_lock);
	}
}


/**
 * Esta funcao vai ler os fifos e encaminhar a ordem a ordem com maior precedencia.
 */
void * read_orders(void* i) {
	char ficheiro[8];
	char *received_file_name = "recebidas.log";
	int pipe_number = *((int *) i);
	sprintf(ficheiro, "fifo%d", pipe_number);
	printf("A ler ordens do %s \n", ficheiro);
	int fd = open(ficheiro, O_RDONLY);		
	for(;;) {
		sem_wait(semaphores[pipe_number]);
		transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
		read(fd, transaction, sizeof(transaction_t));
		pthread_mutex_lock(&queue_lock);
		write_to_file(transaction_print(transaction), received_file_name);
		queue_push(queue, ORDERS, transaction);
		pthread_mutex_unlock(&queue_lock);
		sem_post(&fifos_sem);
	}	
}


/**
 * Inicializa as threads que vão dar inicio ao programa
 */
void start() {
	int number_of_threads = 15;
	int *taskids[10];
	int j = 0;
	pthread_t threads[number_of_threads];
	
	printf("A aguardar por orders.c \n");
    for(int i = 0; i < NUMBER_OF_PROCESSING_THREADS; i++, j++) {
		taskids[i] = (int *) malloc(sizeof(int));
		*taskids[i] = i;
		pthread_create(&threads[j], NULL, process_orders, (void *) taskids[i]);
	}
	
	pthread_create(&threads[++j], NULL, dispatcher_thread, NULL);
    
    for(int i = 0; i < NUMBER_OF_PIPES; i++, j++) {
    	taskids[i] = (int *) malloc(sizeof(int));
    	*taskids[i] = i;
		pthread_create(&threads[j], NULL, read_orders, (void *) taskids[i]);
    } 
	
	
    for(int i = 0; i < number_of_threads; i++) {
		pthread_join(threads[i], NULL);
    } 
	
}


/**
 * Funcao que cria o semaforo da memoria partilhada
 */
void create_sm_semaphore() {		
	shared_memory_sem = sem_open(SHARED_MEMORY_SEMAPHORE, O_CREAT, 00700, 1); 				
}


/**
 * Funcao que inicializa a memoria partilhada
 */
void init_shared_memory() {
	int shared_memory_id = shmget(SHARED_MEMORY_KEY, size_of_shared_memory, IPC_CREAT | FULL_PERMISSIONS);
	if(shared_memory_id < 0) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}
	shared_memory = shmat(shared_memory_id, NULL, 0);
}


/**
 * Cria os semaforos para os repositorios
 */
void create_semaphores() {		
 	char semaphore[10];		
 	for(int i = 0; i < NUMBER_OF_PIPES; i++) {		
 		snprintf(semaphore, sizeof(semaphore), "semafore%d", i);
 		semaphores[i] = sem_open(semaphore, O_CREAT, 00700, 0);  		
 	}		
}


/**
 * Inicializa os semaforos
 */ 
void init_semaphores() {
	sem_init(&fifos_sem, 0, 0);
	for(int i = 0; i < NUMBER_OF_PROCESSING_THREADS; i++) {
		sem_init(&processing_semaphores[i], 0, 0);
	}
}

/**
 * Esta funcao envia o pid do simulator.c para o performace.c através de um pipe.
 */
void * send_pid_to_performance() {
	char *fifo = "performance_pipe";
	FILE *fifo_file = fopen(fifo, "w");
	if ((mkfifo(fifo, FULL_PERMISSIONS) == -1) && (errno != EEXIST)) {
		perror("Error creating named pipe\n");
		exit(EXIT_FAILURE);
	}
	fprintf(fifo_file, "%d", getpid());
	fclose(fifo_file);
}


/**
 * Inicializador do simulador
 */
void setup() {
	pthread_t thread;
	FILE *fifos[NUMBER_OF_PIPES];
	init_semaphores();
	create_semaphores();
	init_shared_memory();
	create_sm_semaphore();
	pthread_create(&thread, NULL, send_pid_to_performance, NULL);
}


/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	setup();
	start();
}
