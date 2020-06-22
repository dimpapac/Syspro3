#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> //dirs
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <signal.h>
#include <errno.h> 
#include <sys/wait.h>	     /* sockets */
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <unistd.h>	         /* fork */		
#include <ctype.h>	         /* toupper */
#include <signal.h>          /* signal */
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>



#include "structs.h"
#include "functions.h"
#include "functions1.h"
#include "list.h"
#include "rbt.h"
#include "heap.h"

#include "whoServer.h"

int flag;
char *servIP;
int servPort;
//mutex and condition variable
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_print = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

void *send_line(void *line){
	pthread_mutex_lock(&mtx);
	while(flag == 0){
		pthread_cond_wait(&cond_var, &mtx);
	}
	pthread_mutex_unlock(&mtx);

	char * ret = connectToServerWithResponse(servIP, servPort, (char *)line);
	printf("%s\n", ret);
	free(ret);
	return NULL;
}

int main(int argc, char *argv[])
{

	if (argc != 9)
	{
		printf("Usage: ./whoServer –p queryFile –w numThreads -sp servPort -sip servIP\n");
		return -1;
	}

	FILE *queryFile;
	int numThreads;
	char *queryFileName;
	int flags = 4;
	for (int i = 1; i < argc; i+=2)
	{
		if (strcmp(argv[i], "-p") == 0)
		{
			queryFileName = malloc(sizeof(char) * (strlen(argv[i+1]) + 1) );
			strcpy(queryFileName, argv[i+1]);
			flags--;
		}
		else if (strcmp(argv[i], "-w") == 0)
		{	
			numThreads = atoi(argv[i+1]);
			flags--;
		}
		else if (strcmp(argv[i], "-sp") == 0)
		{	
			servPort = atoi(argv[i+1]);
			flags--;
		}
		else if (strcmp(argv[i], "-sip") == 0)
		{	
			servIP = malloc(sizeof(char) * (strlen(argv[i+1]) + 1) );
			strcpy(servIP, argv[i+1]);
			flags--;
		}
	}

	// printf("flags %d\n", flags);
	if(flags){
		printf("wrong flags\n");
		return -1;
	}

	// printf("%s %d %d %s\n", queryFileName, numThreads, servPort, servIP);

	flag = 0;

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    queryFile = fopen(queryFileName, "r");
	if (queryFile == NULL) {
		perror("Error opening file\n");
	}

	int numoflines = 0;
	//count queries 
	while ((read = getline(&line, &len, queryFile)) != -1) {
		numoflines++;
		free(line);
		line = NULL;
    }
	free(line);
    line = NULL;

    rewind(queryFile);
    // printf("numoflines %d\n", numoflines);
    char **queries = malloc(sizeof(char*) * numoflines);

    int i = 0;
    while ((read = getline(&line, &len, queryFile)) != -1) {
    	queries[i] = malloc(sizeof(char)* (strlen(line) +1));
    	strcpy(queries[i], line);
		i++;
    }

    if (line)
        free(line);
    fclose(queryFile);


   	pthread_t thr[numoflines]; 
	int err;
	for (int i = 0; i < numoflines; i++)
	{
		if ((err = pthread_create(&thr[i], NULL, send_line, queries[i]))) { /* New thread */
			perror("pthread_create");
			exit(1); 
		}
	}

	flag = 1;
	pthread_cond_broadcast(&cond_var);

	for (int i = 0; i < numoflines; ++i)
	{
		pthread_join(thr[i], NULL);
	}


	return 0;
}