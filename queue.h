/**********************************************************************
 * FICHEIRO: queue.h
 * DESCRICAO:
 *   Este ficheiro contem os defines  e os prototipos das funcoes
 *   necessarios para o funcionamento do queue.c
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

#ifndef QUEUE_H
#define QUEUE_H

/**
 * Esta funcao ordena a lista com recurso ao bubble sort
 */
void queue_sort(transaction_t array[], int max);


/**
 * Esta funcao remove um registo da fila
 */
transaction_t * queue_pop(transaction_t array[], int array_size);


/**
 * Esta funcao imprime a lista
 */
void queue_print(transaction_t array[], int array_size);


/**
 * Esta funcao adiciona um registo a fila
 */
void queue_push(transaction_t array[], int array_size, transaction_t *transaction);

#endif
