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
#include "list.h"
// #include "rbt.h"
// #include "heap.h"

#include "whoServer.h"




int main(int argc, char *argv[])
{

	if (argc != 9)
	{
		printf("Usage: ./whoServer –p queryFile –w numThreads -sp servPort -sip servIP\n");
		return -1;
	}

	FILE *queryFile;
	int numThreads, servPort;
	char *servIP, *queryFileName;
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

	printf("%s %d %d %s\n", queryFileName, numThreads, servPort, servIP);

	

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    queryFile = fopen(queryFileName, "r");
	if (queryFile == NULL) {
		perror("Error opening file\n");
	}

    while ((read = getline(&line, &len, queryFile)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        printf("%s", line);
    }
    printf("\n");
    fclose(queryFile);
    if (line)
        free(line);



	//pthrad_create
	//pthread_join


	return 0;
}