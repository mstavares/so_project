#ifndef TRANSACTION_H
#define TRANSACTION_H
#define NUMBER_OF_TITLES

#include <time.h>

extern char *titles[NUMBER_OF_TITLES];

typedef struct {
	char id[37];
	char title[10];
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
void transaction_clone(transaction_t *destination, transaction_t *source);
char* print_transaction(transaction_t *transaction);
transaction_t * transaction_from_file(FILE *file);

#endif
