/**********************************************************************
 * FICHEIRO: orders.c
 * DESCRICAO:
 *   Este programa cria ordens aleatorias, le ordens atraves de ficheiros
 *   e atraves do teclado. Estas ordens sao escritas em cinco pipes.
 *
 *	 Limpar o ecra
 *	 http://stackoverflow.com/questions/17271576/clear-screen-in-c-and-c-on-unix-based-system
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

#define STDIN_THREAD 0

/** Este array de semaforos e necessario para a comunicacao com o simulator.c */
static sem_t *semaphores[NUMBER_OF_PIPES];

/** Ficheiros onde estao as ordens geradas aleatoriamente */
char *file_names[4] = {"orders1.txt", "orders2.txt", "orders3.txt", "orders4.txt"};

/** Este mutex gere o acesso aos thread ids */
pthread_mutex_t id_lock;

/** Este mutex gere o acesso ao ficheiro das ordens enviadas */
static pthread_mutex_t orders_lock;


/**
 * Funcao que escreve as ordens recebidas
 */
void write_transaction_to_file(transaction_t *transaction) {
	FILE * file = fopen("output/enviadas.log", "a");
	fprintf(file, "%s", transaction_print(transaction));
	fclose(file);
}


/**
 * Esta funcao devolve um id auto incrementavel para cada thread
 */
int get_thread_id() {
	static int fifo_number;
	int thread_id = 0;
	pthread_mutex_lock(&id_lock);
	thread_id = fifo_number;
	fifo_number++;	
	pthread_mutex_unlock(&id_lock);
	return thread_id;
}


/**
 * Esta funcao verifica se um determinado ficheiro existe
 * se sim, retorna um hander para o mesmo.
 */
FILE * is_file_exists(char* file_name) {
	char path[20] = "ficheiros/";
	strcat(path, file_name);	
	return fopen(path, "r");
}


/**
 * Esta funcao le as ordens do ficheiro FILE_NAME_TO_READ e envia as pelo fifo0
 */
void read_orders_from_file(FILE *file, char* ficheiro, int fifo_number, thread_parameter_t *thread_parameter) {
	static int orders_generated;
	int fd = open(ficheiro, O_WRONLY);
	printf("A ler ordens do ficheiro orders[%d].txt \n", fifo_number);
	while(!feof(file) && thread_parameter->number_of_orders > orders_generated) {	
		transaction_t *transaction = transaction_from_file(file);
		if(transaction != NULL) {
			pthread_mutex_lock(&orders_lock);
			write_transaction_to_file(transaction);
			pthread_mutex_unlock(&orders_lock);
			write(fd, transaction, sizeof(transaction_t));
			sem_post(semaphores[fifo_number]);
			orders_generated++;
		}
		sleep(thread_parameter->orders_per_second);
	}
	printf("A leitura do ficheiro orders[%d].txt foi terminada \n", fifo_number);
}


/**
 * Esta funcao escreve as ordens num determinado fifo e aciona o semaforo
 * respeitante ao fifo em questão
 */
void write_orders(int fd, sem_t *sem) {
	for(;;) {
		transaction_t *transaction = transaction_manual_create();
		printf("%s", transaction_print(transaction));
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
void * thread_dispatcher(void* thread_parameter_r) {
	FILE *file;
	char ficheiro[20];
	thread_parameter_t *thread_parameter = (thread_parameter_t *) thread_parameter_r;
	int fifo_number = get_thread_id();
	sprintf(ficheiro, "fifo%d", fifo_number);
	printf("A escrever no %s \n", ficheiro);
	int fd = open(ficheiro, O_WRONLY);
	if(fifo_number == STDIN_THREAD) {
		write_orders(fd, semaphores[fifo_number]);
	} else {
		if(file = is_file_exists(file_names[fifo_number - 1])) {
			read_orders_from_file(file, ficheiro, fifo_number, thread_parameter);
		} else {
			printf("O ficheiro %s nao existe.\n", file_names[fifo_number - 1]);
		}
	}
}


/**
 * Esta função inicializa as threads que vao gerar ordens
 * aleatórias para dentro dos fifos
 */
void start_threads(thread_parameter_t *thread_parameter) {
	pthread_t threads[thread_parameter->number_of_fifos];
	printf("A aguardar pelo simulator.c \n");
	
    for(int i = 0; i < thread_parameter->number_of_fifos; i++) {
    	pthread_create(&threads[i], NULL, thread_dispatcher, (void *) thread_parameter);
	}
	
    for(int i = 0; i < thread_parameter->number_of_fifos; i++) {
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
		if ((mkfifo(fifo, FULL_PERMISSIONS) == -1) && (errno != EEXIST)) {
			perror("Error creating named pipe\n");
			exit(EXIT_FAILURE);
		}
	}
}

void load_uuids() {
	read_uuids_from_file("uuid.txt");
}


/**
 * Esta faz a inicializacao do orders.c
 */
void setup() {
	load_uuids();
	create_fifos();
	create_semaphores();
}


/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	thread_parameter_t *thread_parameter = (thread_parameter_t *) malloc(sizeof(thread_parameter_t));
	
	if(argc < 3 || argc > 5) {
		printf("./orders <numero_de_ordens> <numero_de_ordens_por_segundo> <numero_de_fifos> \n");
	} else if (argc == 4) {
		thread_parameter->number_of_orders = atoi(argv[1]);
		thread_parameter->orders_per_second = atoi(argv[2]);
		thread_parameter->number_of_fifos = atoi(argv[3]);
	} else {
		thread_parameter->number_of_orders = atoi(argv[1]);
		thread_parameter->orders_per_second = atoi(argv[2]);
		thread_parameter->number_of_fifos = 0;
	}
	
	setup();
	start_threads(thread_parameter);	
}
