#ifndef ORDERS_H
#define ORDERS_H

void create_fifos();
void start_threads();
void* generate_orders(void* i);

#endif
