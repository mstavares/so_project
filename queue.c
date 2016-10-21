#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transaction.h"
#include "queue.h"


void queue_push(transaction_t array[], int array_size, transaction_t *transaction) {
	for(int i = 0; i < array_size; i++) {
		if (array[i].value == 0) {
			array[i] = *transaction;
			break;
		}
	}
}


transaction_t * queue_pop(transaction_t array[], int array_size) {
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	transaction_clone(transaction, array);
	for(int i = 0; i < array_size; i++) {
		if(array[i].value == 0) {
			memset(array + i - 1, 0, sizeof(transaction_t));
			break;
		} else {
			memcpy(array + i, array + i + 1, sizeof(transaction_t));
		}
	}
	return transaction;
}


void queue_print(transaction_t array[], int array_size) {
	for(int i = 0; i < array_size; i++) {
		if (array[i].value == 0) {
			break;
		} else {
			printf("%d : %s \n", i, print_transaction( &array[i]) );
		}
	}
}
