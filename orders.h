/**********************************************************************
 * FICHEIRO: orders.h
 * DESCRICAO:
 *   Este ficheiro contem os prototipos das funcoes necessarias para o
 *   funcionamento do orders.c
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

#ifndef ORDERS_H
#define ORDERS_H

/**
 * Esta faz a inicializacao do orders.c
 */
void setup();


/**
 * Esta função cria os fifos para que haja comunicacao com o simulator.c
 */
void create_fifos();


/**
 * Esta funcao cria os semaforos utilizados na comunicacao com
 * o simulator.c
 */
void create_semaphores();


/**
 * Esta é a função invocada pelas threads e é a responsável por gerar
 * as ordens aleatóriamente. Estas ordens são posteriormente inseridas
 * nos fifos.
 */
void * generate_orders(void* i);


/**
 * Esta funcao le as ordens do ficheiro FILE_NAME_TO_READ e envia as pelo fifo0
 */
void read_orders_from_file(FILE *file, char* ficheiro, int fifo_number);


/**
 * Esta função inicializa as threads que vao gerar ordens
 * aleatórias para dentro dos fifos
 */
void start_threads();


/**
 * Esta funcao verifica se um determinado ficheiro existe
 * se sim, retorna um hander para o mesmo.
 */
FILE * is_file_exists(char* file_name);

#endif
