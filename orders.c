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






void* teste(void* recebe) {
	//strcat(ficheiro, ".txt");
	
	int i;
	FILE *f = fopen("teste.bin", "ab");
		
	for(i = 0; i < 10; i++) {
		pthread_mutex_lock(&lock);
		sleep(1);
		transaction_t *transaction = create_transaction();
		fwrite(transaction, sizeof(transaction_t), 1, f);;
		printf("%s \n", print_transaction(transaction));
		pthread_mutex_unlock(&lock);
	}
	
	fclose(f);
	
	/*
	printf("%s \n", "LER!!!");
	
	FILE *r = fopen("teste.bin", "rb");
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	
	for(i = 0; i < 10; i++) {
		fread(transaction, sizeof(transaction_t), 1, r);
		printf("%s \n", print_transaction(transaction));
	}
	
	fclose(r);
	*/
	
	
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
