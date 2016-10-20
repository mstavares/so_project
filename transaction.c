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


/** Este array guarda a lista de empresas existentes na bolsa */
char *title[10] = {"ALTRI", "BPI", "BCP", "CTT", "EDP", "GALP", "NOS", "PHAROL", "REN", "SEMAPA"};


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
 * Esta funcao devolve uma transacao gerada aleatoriamente
 */
transaction_t* create_transaction() {
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	strcpy(transaction->id, random_id());
	strcpy(transaction->title, random_title());
	transaction->value = random_value();
	transaction->amount = random_amount();
	transaction->timestamp = get_timestamp();
	return transaction;
}

/**
 * Esta funcao imprime todos os detalhes de uma transação
 */
char* print_transaction(transaction_t *transaction) {
	static char str[50];
	sprintf(str, "%s %s %f %d %s", transaction->id, transaction->title,
		transaction->value, transaction->amount, timestamp_to_string(transaction->timestamp));
	return str;
}

/**
 * Esta função escolhe aleatoriamente uma empresa da lista de titulos
 * O inteiro devolvido estará entre 0 e 9
 */
char* random_title() {
	return title[rand() % 10];
}

/**
 * Esta função escolhe aleatoriamente a quantidade de acoes a ser transacionada
 * O inteiro devolvido estará entre 0 e 99
 */
int random_amount() {
	return rand() % 100;
}

/**
 * Esta função escolhe aleatoriamente o preço das ações
 * O float devolvido estará entre 
 */
float random_value() {
	return (float) (rand() % 20 - 10);
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


