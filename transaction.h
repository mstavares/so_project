/**********************************************************************
 * FICHEIRO: transaction.h
 * DESCRICAO:
 *   Este ficheiro contem os defines, as estruturas e os prototipos das 
 *   funcoes necessarios para o funcionamento do transaction.c
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

#ifndef TRANSACTION_H
#define TRANSACTION_H

#define NUMBER_OF_TITLES 10
#include <time.h>

/** Este array guarda a lista de empresas existentes na bolsa */
extern char *titles[NUMBER_OF_TITLES];

typedef struct {
	char id[37];
	char title[10];
	float value;
	int amount;
	time_t timestamp;
}transaction_t;

/**
 * Esta função devolve um timestamp no formato long
 */
char* timestamp_to_string(time_t now);


/**
 * Esta função devolve um timestamp no formato long
 */
long get_timestamp();


/**
 * Esta função escolhe aleatoriamente o preço das ações
 * O float devolvido estará entre 1 e 20
 */
float random_value();

/**
 * Esta função escolhe aleatoriamente a quantidade de acoes a ser transacionada
 * O inteiro devolvido estará entre 1 e 100
 */
int random_amount();


/**
 * Esta função escolhe aleatoriamente uma empresa da lista de titulos
 * O inteiro devolvido estará entre 0 e 9
 */
char* random_title();


/**
 * Esta funcao imprime todos os detalhes de uma transação
 */
char* transaction_print(transaction_t *transaction);


/**
 * Esta funcao permite clonar transacoes
 */
void transaction_clone(transaction_t *destination, transaction_t *source);


/**
 * Esta funcao devolve uma transacao gerada aleatoriamente
 */
transaction_t * transaction_create();


/**
 * Esta funcao devolve uma transacao tendo como base o input do utilizador
 */
transaction_t * transaction_manual_create();


/**
 * Esta funcao permite ler transacoes atraves de um ficheiro
 */
transaction_t * transaction_from_file(FILE *file);


/**
 * Esta função cria um UUID aleatoriamente
 * Os UUID random têm a forma xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
 */
char* random_id();

/**
 * Esta funcao converte os caracteres das strings para maiusculas
 */
void string_to_upper_case(char *string);

#endif
