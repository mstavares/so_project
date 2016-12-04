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

char uuid[NUMBER_OF_UUID][SIZE_OF_UUID];

int uuids_allocated = 0;


void read_uuids_from_file(char* file_name) {
	char path[20] = "ficheiros/";
	strcat(path, file_name);	
	FILE *file = fopen(path, "r");

	while(!feof(file)) {
		fscanf(file, "%s", uuid[uuids_allocated]);
		uuids_allocated++;
	}
	
}


/**
 * Esta função cria um UUID aleatoriamente
 * Os UUID random têm a forma xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
 */
char* random_id() {
	return uuid[rand() % uuids_allocated];
}


/**
 * Esta funcao permite ler transacoes atraves de um ficheiro
 */
transaction_t * transaction_from_file(FILE *file) {
	transaction_t *transaction = (transaction_t *) malloc(sizeof(transaction_t));
	int bytes = fscanf(file, "%s %f %d", transaction->title, &transaction->value, &transaction->amount);
	strcpy(transaction->id, random_id());
	strcpy(transaction->timestamp, get_time());
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
	strcpy(transaction->timestamp, get_time());
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
	strcpy(transaction->timestamp, get_time());
	return transaction;
}


/**
 * Esta funcao imprime todos os detalhes de uma transação
 */
char* transaction_print(transaction_t *transaction) {
	static char str[50];
	sprintf(str, "%s %s %.2f %d %s \n", transaction->id, transaction->title,
		transaction->value, transaction->amount, transaction->timestamp);
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
 * Esta função devolve um timestamp no formato char*
 */
char* get_time() {
    static char buff[100];
    char milisec[10];
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    strftime(buff, sizeof buff, "%Y-%m-%d %T", gmtime(&ts.tv_sec));
    sprintf(milisec, ".%ld", ts.tv_nsec);
    strcat(buff, milisec);
    return buff;
}


/**
 * Esta funcao converte os caracteres das strings para maiusculas
 */
void string_to_upper_case(char *string) {
	for(; *string != '\0'; string++) {
		*string = toupper(*string);
	}
}


