/**********************************************************************
 * FICHEIRO: simulator.h
 * DESCRICAO:
 *   Este ficheiro contem os protopicos e os defines necessarios para o
 *   simulator.c
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

#ifndef SIMULATOR_H
#define SIMULATOR_H

/** Numero de ordens que podem ser processadas no repositorio */
#define ORDERS 100000

/** Nome do semaforo da memoria partilhada */
#define SHARED_MEMORY_SEMAPHORE "shared_memory_semafore"

/** Chave da memoria partilhada */
#define SHARED_MEMORY_KEY 1234


/**
 * Inicializador do simulador
 */
void setup();


/**
 * Esta funcao envia o pid do simulator.c para o performace.c através de um pipe.
 */
void * send_pid_to_performance();


/**
 * Esta funcao abre os fifos para leitura.
 */
void open_fifos(FILE *fifos[]);


/**
 * Inicializa os semaforos
 */
void init_semaphores();


/**
 * Cria os semaforos para os repositorios
 */
 void create_semaphores();
 
 
 /**
 * Funcao que inicializa a memoria partilhada
 */
void init_shared_memory();


/**
 * Funcao que cria o semaforo da memoria partilhada
 */
void create_sm_semaphore();


/**
 * Inicializa as threads que vão dar inicio ao programa
 */
void start();


/**
 * Inicializa as 10 threads que vão processar os repositórios de ordens
 */
void* start_processing_threads();


/**
 * Esta função inicializa as threads que vao ler as ordens
 * contidas nos fifos
 */
void* start_reading_threads();


/**
 * Esta funcao vai ler os fifos e encaminhar a ordem a ordem com maior precedencia.
 */
void* read_orders(void* i);


/**
 * Inicializa a dispatcher thread
 */
void* start_dispatcher_thread();


/**
 * A dispatcher thread é responsável por:
 * 1: Ler as ordens dos 5 fifos
 * 2: Entregar a ordem no repositorio de ordens correspondente
 * 3: Escrever na memoria partilhada
 * 4: Acionar o semaforo da memoria partilhada
 */
void* dispatcher_thread();


/**
 * Faz o trabalho da dispatcher thread
 */
void allocate_orders(transaction_t *transaction);


/**
 * Funcao que escreve na memoria partilhada
 */
void write_on_shared_memory(transaction_t array[]);


/**
 * Thread que processa os repositorios e escreve no ficheiro de logs
 */
void* process_orders(void *received);


/**
 * Funcao que escreve no ficheiro de logs
 */
void write_to_file(char* string_to_write, char* file_name);


#endif
