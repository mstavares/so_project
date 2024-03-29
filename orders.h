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

/** Numero de pipes para a comunicacao entre o orders.c e o simulator.c */
#define NUMBER_OF_PIPES 5

/** Define as permissoes de acesso aos fifos */
#define FULL_PERMISSIONS 0666

/** Esta estrutura serve para passar mais do que um parametro para as threads */
typedef struct {
	int number_of_orders;
	int orders_per_second;
	int number_of_fifos;
}thread_parameter_t;

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
 * Esta função inicializa as threads que vao gerar ordens
 * aleatórias para dentro dos fifos
 */
void start_threads();


/**
 * Esta funcao vai verificar se:
 * 1: Temos ordens para ler a partir de ficheiros
 * 2: Não temos ficheiros vai gerar ordens aleatorias
 * 3: Temos de criar a thread para o utilizador criar ordens
 */
void * thread_dispatcher(void* i);


/**
 * Esta funcao escreve as ordens num determinado fifo e aciona o semaforo
 * respeitante ao fifo em questão
 */
void write_orders(int fd, sem_t *sem);


/**
 * Esta funcao le as ordens do ficheiro FILE_NAME_TO_READ e envia as pelo fifo0
 */
void read_orders_from_file(FILE *file, char* ficheiro, int fifo_number, thread_parameter_t *thread_parameter);


/**
 * Esta funcao verifica se um determinado ficheiro existe
 * se sim, retorna um hander para o mesmo.
 */
FILE * is_file_exists(char* file_name);


/**
 * Esta funcao devolve um id auto incrementavel para cada thread
 */
int get_thread_id();


/**
 * Funcao que escreve as ordens recebidas
 */
void write_transaction_to_file(transaction_t *transaction);


#endif
