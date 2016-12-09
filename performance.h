/**********************************************************************
 * FICHEIRO: performance.h
 * DESCRICAO:
 *   Este ficheiro contem os prototipos e os defines necessarios para o
 *   performance.c
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

#ifndef PERFORMANCE_H
#define PERFORMANCE_H


/**
 * Esta funcao recebe o pid do simulator através de um pipe.
 * Este pid é utilizado para apresentar a performance do simulator.
 */
int receive_simulator_pid();
 
 
/**
 * Esta funcao vai fazer o parse do ps aux e apresentar apenas o CPU e o MEM
 * do pid recebido.
 */
void print_performance(char *simulator_pid, int refreshing_time);


/**
 * Esta funcao inicializa o programa performance
 */
void start();


#endif
