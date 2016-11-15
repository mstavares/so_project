/**********************************************************************
 * FICHEIRO: share.c
 * DESCRICAO:
 *   Este programa le as ordens dos repositorios atraves da memoria
 *   partilhada
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include "transaction.h"
#include "simulator.h"
#include "queue.h"
#include "share.h"

#define NUMBER_OF_SEMAPHORES 1
#define PERMISSIONS 0666

/** Este semaforo e utilizado e acionado pelo simulator.c */
static sem_t *sem;

/** E atraves deste array de chars que lemos a memoria partilhada */
static char* shared_memory;

/** Esta variavel contem o tamanho em bytes da memoria partilhada */
int size_of_shared_memory = sizeof(transaction_t) * ORDERS;


/**
 * Esta funcao le da memoria partilhada e escreve no ecra.
 * Esta leitura e acionada com recurso a um semaforo partilhado
 * entre o share.c e o simulator.c
 */
void read_shared_memory() {
	transaction_t data_read_from_sm[ORDERS];
	for(;;){
		sem_wait(sem);
		memcpy(data_read_from_sm, shared_memory, size_of_shared_memory);
		printf("*******************************************************\n");
		queue_print(data_read_from_sm, ORDERS);
	}
}

/**
 * Esta funcao inicializa a memoria partilhada
 */
 void init_shared_memory() {
	key_t shared_memory_key = SHARED_MEMORY_KEY;

	int shared_memory_id = shmget(shared_memory_key, size_of_shared_memory, IPC_CREAT | PERMISSIONS);
	
	if(shared_memory_id < 0) {
		perror("shmget");
		exit(1);
	}
	
	shared_memory = shmat(shared_memory_id, NULL, 0);
	
	if(shared_memory == (char* ) -1) {
		perror("shmat");
		exit(1);
	}
}


/**
 * Esta funcao inicializa o semaforo da memoria partilhada
 */
void create_semaphore() {		
	sem = sem_open(SHARED_MEMORY_SEMAPHORE, O_CREAT, 00700, 1); 				
}


/**
 * Esta faz as inicializacoes necessarias para o funcionamento do
 * programa
 */
 void setup() {
	create_semaphore();
	init_shared_memory();
}


/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	setup();
	read_shared_memory();
	return 0;
}
