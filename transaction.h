#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <time.h>

extern char str[36];
extern char *title[10];

typedef struct {
	char id[36];
	char title[6];
	float value;
	int amount;
	char timestamp[50];
}transaction_t;

char* random_id();
char* random_title();
int random_amount();
float random_value();
char* get_timestamp();
transaction_t* create_transaction();
char* print_transaction(transaction_t *transaction);

#endif
