#ifndef QUEUE_H
#define QUEUE_H
#define NUMBER_OF_TITLES

void queue_push(transaction_t array[], int array_size, transaction_t *transaction);
transaction_t * queue_pop(transaction_t array[], int array_size);
void queue_print(transaction_t array[], int array_size);

#endif
