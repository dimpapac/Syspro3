#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //dirs
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <signal.h>
#include <errno.h> 




#include "structs.h"
#include "functions.h"
#include "list.h"
// #include "rbt.h"
// #include "heap.h"


extern int pflag;
extern int ptotal;
extern int psuccess;
extern int pfail;

void handler(int sig){
	// signal(SIGINT, handler);
	// signal(SIGQUIT, handler);
	// printf("diseaseAggregator: I have received a sigint/sigquit\n");
	pflag = 1;
	// printf("flag in handler %d\n", pflag);
}



int main(int argc, char *argv[])
{

	pflag = 0;
	ptotal = 0;
	psuccess = 0;
	pfail = 0;
	signal(SIGINT, handler);
	signal(SIGQUIT, handler);

	if (argc != 7)
	{
		printf("Usage: ./diseaseAggregator -w numWorkers -b bufferSize -i input_dir\n");
		return -1;
	}

	// struct dirent *dir;  // Pointer for directory entry
	int bufferSize, numWorkers;
	char *input_dir;
	int flags = 3;
	for (int i = 1; i < argc; i+=2)
	{
		if (strcmp(argv[i], "-w") == 0)
		{
			numWorkers = atoi(argv[i+1]);
			flags--;
		}
		else if (strcmp(argv[i], "-b") == 0)
		{	
			bufferSize = atoi(argv[i+1]);
			flags--;
		}
		else if (strcmp(argv[i], "-i") == 0)
		{	
			input_dir = malloc(sizeof(char) * (strlen(argv[i+1]) + 1) );
			strcpy(input_dir, argv[i+1]);
			flags--;
		}
	}

	// printf("flags %d\n", flags);
	if(flags){
		printf("wrong flags\n");
		return -1;
	}

	// printf("numWorkers %d\n", numWorkers);
	// printf("bufferSize %d\n", bufferSize);
	// printf("input_dir %s\n", input_dir);


	char **readFifosNames = malloc(sizeof(char *) * numWorkers); //hold names of pipes for reading 
	char **writeFifosNames = malloc(sizeof(char *) * numWorkers); //hold names of pipes for writing
	


	paths_list_node **list_head = malloc(sizeof(paths_list_node *) * numWorkers); //head of path lists
	for (int i = 0; i < numWorkers; i++)
		list_head[i] = NULL;


	int *pids;
	pids = malloc(sizeof(int) * numWorkers); // pids of children
	// int fifos[numWorkers];
	int *fifosR= malloc(sizeof(int) * numWorkers);
	int *fifosW= malloc(sizeof(int) * numWorkers);
	// printf("NUMBER OF Workers %d\n", numWorkers);
	printf("Waiting for statistics...\n");
	for (int i = 0; i < numWorkers; i++){ //for each worker
		
		readFifosNames[i] = malloc((strlen("/tmp/pipeR") + 3) * sizeof(char));
		sprintf(readFifosNames[i], "%s%d", "/tmp/pipeR", i); //make named pipe's name
		// printf("%s ------------------- %d\n", readFifosNames[i], i);
		if (mkfifo(readFifosNames[i],0666) == -1){ //make fifo
			perror("fifo error");
			return -1;
		}

		writeFifosNames[i] = malloc((strlen("/tmp/pipeW") + 3) * sizeof(char));
		sprintf(writeFifosNames[i], "%s%d", "/tmp/pipeW", i); //make named pipe's name
		// printf("%s ------------------- %d\n", writeFifosNames[i], i);
		if (mkfifo(writeFifosNames[i],0666) == -1){ //make fifo
			perror("fifo error");
			return -1;
		}

		
		pid_t pid;
		if ((pid = fork()) < 0){ //create a new process for the worker
			perror("failed to fork");
			return -1;
		}

		if (pid == 0){ //child process
		
			char * array[8]; //array for execv
			array[0] = "Worker"; //name of input file
			array[1] = malloc(sizeof(char) + 3); //number of worker
			sprintf(array[1], "%d", i);
			array[2] = malloc(sizeof(char)*(strlen(readFifosNames[i])+1)); //number of read pipe
			strcpy(array[2], readFifosNames[i]);
			array[3] = malloc(sizeof(char)*(strlen(writeFifosNames[i])+1)); //number of write pipe
			strcpy(array[3], writeFifosNames[i]);
			array[4] = malloc(sizeof(char)*5); //bufferSize
			sprintf(array[4], "%d", bufferSize);
			array[5] = NULL; 
			if (execvp("./Worker", array) < 0) {    
				perror("*** ERROR: exec failed\n");
				free(array[1]);
				free(array[2]);
				free(array[3]);
				free(array[4]);
				exit(1);
			}
			printf("kalispera apo worker meta tin exec%d\n", i);
			return 0;
		}
		else {
			// printf("%d\n", pid);
			pids[i] = pid;
			// wait(NULL); // perimenw na teleiwsei o kathenas gia na sinexisw 
			// kill(pid, SIGUSR2);
		}
	}

	for (int i = 0; i < numWorkers; i++)
	{
		// printf("process id: %d\n", pids[i]);
    	// sleep(2);
    	// ret = kill(pids[i], SIGUSR2); //send a signal that i have finished


    	// printf("%s\n", readFifosNames[i]);
		if((fifosR[i] = open(readFifosNames[i], O_RDONLY | O_NONBLOCK)) < 0){ 
			perror("error opening read pipes\n");
		}

		// error checking for fcntl 
    	// if (fcntl(fifosR[i], F_SETFL, O_NONBLOCK) < 0) {
    	// 	perror("fnctl fifoR");
    	// 	return -1; 
    	// }
    	// printf("%s\n", writeFifosNames[i]);
    	sleep(1); //wait for read end to open first
		if((fifosW[i] = open(writeFifosNames[i], O_WRONLY | O_NONBLOCK)) < 0){ 
			perror("error opening write pipes\n");
		}

		// if (fcntl(fifosW[i], F_SETFL, O_NONBLOCK) < 0) {
  //   		perror("fnctl fifoW");
		// 	return -1; 	
		// }
	}



	// kill(pids[0], SIGUSR2);
	// kill(pids[1], SIGUSR2);

	DIR *dir;
    struct dirent *entry;
    if (!(dir = opendir(input_dir))){
    	perror("opendir");
        return -1;
    }

    int counter = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char *path = malloc(strlen(input_dir) + strlen(entry->d_name) + 2); //size of dir name
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            	free(path);
                continue;
            }
            sprintf(path, "%s/%s", input_dir, entry->d_name);
            path[strlen(path)] = '\0'; 
            // printf("ENTRY->D_NAME   %s\n", entry->d_name);
			append_path_list(&list_head[counter], entry->d_name);

            // printf("%s\n", path);
            // snprintf(sizeOfMessage, sizeof(int), "%d", strlen(path));
            // printf("sizeOfMessage   %s\n", sizeOfMessage);
            int size = (int) strlen(path) + 1;
            // printf("sizeOfMessage dA  -- %d\n", size);
           	//write size of message 
           	if (write(fifosW[counter], &size, sizeof(int)) == -1){ 
				perror("write");
				return -1;
			}
			// sleep(1);

			// // write message
            if (write(fifosW[counter], path, strlen(path) + 1) == -1){ 
				perror("write");
				return -1;
			}
			sleep(1);
			free(path);

			counter++;
			if (counter == numWorkers) 
				counter = 0;

        }
    }
    closedir(dir);

    char *EOM = "EOM";
    for (int i = 0; i < numWorkers; i++)
	{
		int size = (int) strlen(EOM) + 1;
        // printf("sizeOfMessage dA  -- %d\n", size);
       	//write size of message 
       	if (write(fifosW[i], &size, sizeof(int)) == -1){ 
			perror("write");
			return -1;
		}

	    if (write(fifosW[i], EOM, (sizeof(char) * strlen(EOM)) + 1) == -1){ 
			perror("write EOM");
			return -1;
		}

	}
	


	/*non-blocking pipe*/
	int size = 0; 
	// int ret = 0;
	char stats[1000];
    for (int i = 0; i < numWorkers; i++)
	{
        while(1)
		{
		   	while (read(fifosR[i], &size, sizeof(int)) < 0){ 
		   		// if (errno == EAGAIN) { 
			    //     printf("(pipe empty)\n"); 
			    //     sleep(1);
			    //     // break; 
			    // }
			    // printf("PAMELIGO\n");
			}
	    	// printf("sizeOfMessage in worker  %d  ----pid: %d\n", size, getpid());
	    	// printf("size %d\n", size);
	    	// break;
	    	if (size == -15)
	    	{
	    		// printf("TELOS\n");
	    		break;
	    	}
	    	// sleep(1);

	    	while (read(fifosR[i], stats, size) < 0){ 
		   		// if (errno == EAGAIN) { 
			    //     printf("(pipe empty)\n"); 
			    //     sleep(1);
			    //     // break; 
			    // }
			    // printf("PAMELIGO\n");
			}

			printf("%s\n", stats);

		}

	}

	// printf("PROCESS ID: %d\n", getpid());

	/*non-blocking pipe*/


    cli(list_head, pids, numWorkers, fifosR, fifosW);


	// printf("unlink of diseaseAggregator\n");
	for (int i = 0; i < numWorkers; i++){
		wait(NULL);		//wait for child processes
		if (unlink(readFifosNames[i]) < 0){
			perror("fifo unlink error");
			return -1;
		}
		if (unlink(writeFifosNames[i]) < 0){
			perror("fifo unlink error");
			return -1;
		}	
	}
	
	char *logfile = malloc(sizeof(char) * (strlen("log_file.") + 10));

	sprintf(logfile, "log_file.%d", getpid());
	FILE * fp;
	fp = fopen(logfile, "w");
	for (int i = 0; i < numWorkers; i++)
	{
		paths_list_node *cur = list_head[i];
		while(cur != NULL){
			// printf("%s\n", cur->path);
			fprintf(fp, "%s\n", cur->path);
			cur = cur->next;
		}
	}

	fprintf(fp, "TOTAL %d\n", ptotal);
	fprintf(fp, "SUCCESS %d\n", psuccess);
	fprintf(fp, "FAIL %d\n", pfail);

   	fclose(fp);

   	for (int i = 0; i < numWorkers; i++)
   	{
	   	free(readFifosNames[i]); 
	   	free(writeFifosNames[i]); 
   		free_path_list(list_head[i]);
   	}

   	free(input_dir);
   	free(pids);
   	free(readFifosNames);
   	free(writeFifosNames);
   	free(list_head);
   	free(fifosR);
   	free(fifosW);
	free(logfile);
	printf("diseaseAggregator exiting...\n");
	return 0;	
}