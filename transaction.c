/**********************************************************************
 * FICHEIRO: transaction.c
 * DESCRICAO:
 *   Este ficheiro representa as transacoes e tem as funcoes que lhe
 *	 sao respetivas
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

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

#define stdout_clear() while(scanf("%*c"))


/** Este array guarda a lista de empresas existentes na bolsa */
char *titles[NUMBER_OF_TITLES] = {"ALTRI", "BPI", "BCP", "CTT", "EDP", "GALP", "NOS", "PHAROL", "REN", "SEMAPA"};


/**
 * Esta função cria um UUID aleatoriamente
 * Os UUID random têm a forma xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
 */
char* random_id() {
	static char str[37]; 
	uuid_t uuid;
	uuid_clear(uuid);
	uuid_generate_random(uuid);
	uuid_unparse(uuid,str); 
	return str; 
}


/**
 * Esta funcao permite ler transacoes atraves de um ficheiro
 */
transaction_t * transaction_from_file(FILE *file) {
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	int bytes = fscanf(file, "%s %f %d", transaction->title, &transaction->value,
			&transaction->amount);
	strcpy(transaction->id, random_id());
	transaction->timestamp = get_timestamp();
	if(bytes < 0) {
		transaction = NULL;
	}
	return transaction;
}


/**
 * Esta funcao devolve uma transacao gerada aleatoriamente
 */
transaction_t * transaction_create() {
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	strcpy(transaction->id, random_id());
	strcpy(transaction->title, random_title());
	transaction->value = random_value();
	transaction->amount = random_amount();
	transaction->timestamp = get_timestamp();
	return transaction;
}


/**
 * Esta funcao devolve uma transacao tendo como base o input do utilizador
 */
transaction_t * transaction_manual_create() {
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	char title[50];
	strcpy(transaction->id, random_id());
	printf("Introduza o nome de uma empresa cotada em bolsa: \n");
	scanf("%s", title);
	string_to_upper_case(title);
	strcpy(transaction->title, title);
	printf("Introduza o numero de acoes: \n");
	scanf("%d", &transaction->amount);
	printf("Introduza o valor da transacao: \n");
	scanf("%f", &transaction->value);
	transaction->timestamp = get_timestamp();
	return transaction;
}


/**
 * Esta funcao permite clonar transacoes
 */
void transaction_clone(transaction_t *destination, transaction_t *source) {
	strcpy(destination->id, source->id);
	strcpy(destination->title, source->title);
	destination->value = source->value;
	destination->amount = source->amount;
	destination->timestamp = source->timestamp;
}


/**
 * Esta funcao imprime todos os detalhes de uma transação
 */
char* transaction_print(transaction_t *transaction) {
	static char str[50];
	sprintf(str, "%s %s %.2f %d %s", transaction->id, transaction->title,
		transaction->value, transaction->amount, timestamp_to_string(transaction->timestamp));
	return str;
}


/**
 * Esta função escolhe aleatoriamente uma empresa da lista de titulos
 * O inteiro devolvido estará entre 0 e 9
 */
char* random_title() {
	return titles[rand() % 10];
}


/**
 * Esta função escolhe aleatoriamente a quantidade de acoes a ser transacionada
 * O inteiro devolvido estará entre 1 e 100
 */
int random_amount() {
	return (rand() % 100) + 1;
}


/**
 * Esta função escolhe aleatoriamente o preço das ações
 * O float devolvido estará entre 1 e 20
 */
float random_value() {
	return (float) (rand() % 20 - 10) + 1;
}


/**
 * Esta função devolve um timestamp no formato long
 */
long get_timestamp() {
	time_t now = time(NULL);
	return now;
}


/**
 * Esta função devolve um timestamp no formato long
 */
char* timestamp_to_string(time_t now) {
	return asctime (localtime (&now));
}


/**
 * Esta funcao converte os caracteres das strings para maiusculas
 */
void string_to_upper_case(char *string) {
	for(; *string != '\0'; string++) {
		*string = toupper(*string);
	}
}


