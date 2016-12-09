/**********************************************************************
 * FICHEIRO: share.h
 * DESCRICAO:
 *   Este ficheiro contem os prototicos e os defines necessarios para o
 *   share.c
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

#ifndef SHARE_H
#define SHARE_H


/**
 * Esta faz as inicializacoes necessarias para o funcionamento do
 * programa
 */
 void setup();
 
 
 /**
 * Esta funcao inicializa o semaforo da memoria partilhada
 */
void create_semaphore();


/**
 * Esta funcao inicializa a memoria partilhada
 */
 void init_shared_memory();
 
 
 /**
 * Esta funcao le da memoria partilhada e escreve no ecra.
 * Esta leitura e acionada com recurso a um semaforo partilhado
 * entre o share.c e o simulator.c
 */
void read_shared_memory();


#endif
