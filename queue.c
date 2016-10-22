#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "transaction.h"
#include "queue.h"

#define FALSE 0
#define TRUE 1

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
			printf("%d : %s \n", i, transaction_print(&array[i]));
		}
	}
}

void queue_sort(transaction_t array[], int max) {
	transaction_t temp;
	int swapped = FALSE;
		for(int i = 0; i < max - 1; i++) { 
			swapped = FALSE;
			for(int j = 0; j < max - 1 - i; j++) {
				if(array[j+1].value == 0) {
					swapped = TRUE;
				} else {
					if(fabs(array[j].value) > fabs(array[j + 1].value)) {
						temp = array[j];
						array[j] = array[j+1];
						array[j+1] = temp;
						swapped = TRUE;
					}
				}				
			}
		if(!swapped) {
			break;
		}
	}
}
