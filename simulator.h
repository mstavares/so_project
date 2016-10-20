#ifndef SIMULATOR_H
#define SIMULATOR_H

void create_fifos();
fd_set create_pool();
void read_orders(FILE *fifos[]);

#endif
