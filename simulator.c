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


void *readOrders(void *ficheiro)
{
	FILE *r = fopen("teste.bin", "rb");
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
        	
	while (fread(transaction, sizeof(transaction_t), 1, r)) {
		pthread_mutex_lock(&lock);
		sleep(1);
		//fread(transaction, sizeof(transaction_t), 1, r);
		printf("%s \n", print_transaction(transaction));
		pthread_mutex_unlock(&lock);
	}
	    
	//pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

	readOrders("teste");
/*	
	pthread_t thread;
	pthread_create(&threads[t], NULL, readOrders, (void *) );
	pthread_join(thread, NULL);
*/
}
