#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <time.h>

extern char str[36];
extern char *title[10];

typedef struct {
	char id[37];
	char title[6];
	float value;
	int amount;
	time_t timestamp;
}transaction_t;

char* random_id();
char* random_title();
int random_amount();
float random_value();

long get_timestamp();
char* timestamp_to_string(time_t now);

transaction_t* create_transaction();
char* print_transaction(transaction_t *transaction);

#endif
