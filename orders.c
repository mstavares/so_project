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




void* teste(void* recebe) {
	//strcat(ficheiro, ".txt");
	
	
	int i;
	for(i = 0; i < 10; i++) {
		sleep(1);
		transaction_t *transaction = create_transaction();
		printf("%s \n", print_transaction(transaction));
	}
	
	
	/*
	char *ficheiro = (char *) recebe;
	int i;
	
	FILE *f = fopen("teste.txt", "a");
	for(i = 0; i < 5; i++) {
		fputs(ficheiro, f);
		fputs("\n", f);
	}
	fclose(f);
*/
		
}

/**
 *
 */
void start(int number_of_orders) {
	pthread_t threads[NUM_THREADS];
	int rc, t;
    int *taskids[NUM_THREADS];
    int i;
    
    char* palavras[4] = {"thread1", "thread2", "thread3", "thread4"}; 
    
    for(i = 0; i < 4; i++) {
		pthread_create(&threads[i], NULL, teste, (void *) palavras[i]);
		//pthread_join(threads[i], NULL);
    }
    
    
    for(i = 0; i < 4; i++) {
		pthread_join(threads[i], NULL);
    }
    
    
    
    
    /*
    char* palavras[4] = {"thread1", "thread2", "thread3", "thread4"}; 
    
    for(i = 0; i < 4; i++) {
    	teste(palavras[i]);
    }
    */
    
    /*
    char lala[50] = "teste";
    teste(lala);
    */
    
    
    /*
    for(i = 0; i < 4; i++) {
    	//teste(  );
		char str[36]; 
		uuid_t uuid;
		uuid_clear(uuid);
		uuid_generate_random(uuid);
		uuid_unparse(uuid,str);
		teste("teste");
    }
    */
    
	/** Cria fifos caso não existam */
	/*
	if (access("fifo0", F_OK) == -1) 
		create_pipes();
		*/
}

/**
 * Inicio do programa
 */
int main(int argc, char* argv[]) {
	if(argc == 2) {
		start(atoi(argv[1]));
	} else {
		printf("O executavel nao foi invocado devidamente. \n");
		printf("Deve usar a seguinte sintaxe: ./orders <numero de ordens> \n");
	}
}
