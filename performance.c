#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <fcntl.h>

#define REFRESHING_TIME 2


int receive_simulator_pid() {
	int simulator_pid;
	char* fifo = "performance_pipe";
	FILE *fifo_file = fopen(fifo, "r");
	fscanf(fifo_file, "%d", &simulator_pid);
	fclose(fifo_file);
	return simulator_pid;
}

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

void start() {
	char simulator_pid[10];
	printf("Waiting for simulator.c openning.\n");
	sprintf(simulator_pid, "%d", receive_simulator_pid());
	print_performance(simulator_pid, REFRESHING_TIME);
}

int main() {
	start();	
}
