/**********************************************************************
 * FICHEIRO: performance.c
 * DESCRICAO:
 *   Este ficheiro apresenta ao utilizador a performance do programa simulator
 *
 * AUTOR: Miguel Tavares - 21304351
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include "performance.h"

#define REFRESHING_TIME 2

/**
 * Esta funcao recebe o pid do simulator através de um pipe.
 * Este pid é utilizado para apresentar a performance do simulator.
 */
int receive_simulator_pid() {
	int simulator_pid;
	char* fifo = "performance_pipe";
	FILE *fifo_file = fopen(fifo, "r");
	fscanf(fifo_file, "%d", &simulator_pid);
	fclose(fifo_file);
	return simulator_pid;
}


/**
 * Esta funcao vai fazer o parse do ps aux e apresentar apenas o CPU e o MEM
 * do pid recebido.
 */
void print_performance(char *simulator_pid, int refreshing_time) {
	FILE *in;
	char buff[512];
	char *split;
	
	for(;;) {
		if(!(in = popen("ps aux", "r"))){
			exit(EXIT_FAILURE);
		}
		while(fgets(buff, sizeof(buff), in)!=NULL){
			split = strtok(buff," ");
			 while (split != NULL) {
			 	if(strcmp(split, simulator_pid) == 0) {
			 		printf("\033[H\033[J");
			 		printf("%%CPU %%MEM \n");
			 		for(int i = 0; i < 2; i++) {
			 			split = strtok(NULL, " ");
			 			printf("%s ", split);
			 		}
			 		printf("\n");
			 		break;
			 	}
			 	split = strtok(NULL, " ");
			 }
		}
		pclose(in);
		sleep(refreshing_time);
	}
}


/**
 * Esta funcao inicializa o programa performance
 */
void start() {
	char simulator_pid[10];
	printf("Waiting for simulator.c openning.\n");
	sprintf(simulator_pid, "%d", receive_simulator_pid());
	print_performance(simulator_pid, REFRESHING_TIME);
}


/**
 * Inicia o programa
 */
int main() {
	start();	
}
