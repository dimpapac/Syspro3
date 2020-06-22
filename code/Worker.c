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

#include "structs.h"
#include "functions.h"
#include "functions1.h"
#include "list.h"
#include "rbt.h"
#include "heap.h"

paths_list_node *path_head = NULL; //head of path list

extern int ctotal;
extern int csuccess;
extern int cfail;

//function to handle the SIGUSR2 signal 

void handler(){
	printf("WORKER: I have received a SIGINT/SIGQUIT\n");
	// signal(SIGUSR2, handler);
	// signalsReceived++;

	char *logfile = malloc(sizeof(char) * (strlen("log_file.") + 10));

	sprintf(logfile, "log_file.%d", getpid());
	FILE * fp;
	fp = fopen(logfile, "w");

	paths_list_node *cur = path_head;
	while(cur != NULL){
		// printf("%s\n", cur->path);
		char * token = strtok(cur->path, "/");
		token = strtok(NULL, " ");
		fprintf(fp, "%s\n", token);
		cur = cur->next;
	}

	fprintf(fp, "TOTAL %d\n", ctotal);
	fprintf(fp, "SUCCESS %d\n", csuccess);
	fprintf(fp, "FAIL %d\n", cfail);

   	fclose(fp);

	exit(1);
}


int main(int argc, char *argv[])
{

	// printf("kalispera apo worker %d\n", getpid());
	// printf("%s%s and name of pipe is--- %s --- %s \n", argv[0], argv[1], argv[2], argv[3], argv[4]);
	// printf("argv0 %s\n", argv[0]);
	// printf("argv1 %s\n", argv[1]);
	// printf("argv2 %s\n", argv[2]);
	// printf("argv3 %s\n", argv[3]);
	// printf("argv4 %s\n", argv[4]);
	// printf("BUFFER SIZE IS %d\n", bufferSize);

	// signal(SIGINT, handler);
	// signal(SIGQUIT, handler);
	ctotal = 0;
	csuccess = 0;
	cfail = 0;

	// char *write_pipe = malloc(sizeof(char)* strlen(argv[2]))

	int bufferSize = atoi(argv[4]); 
	int fifosR, fifosW;
	char *buffer = malloc(sizeof(char) * bufferSize + 1); //+1 added to store the null terminator 

	//argv[3] is pipe for reading and argv[2] for writing 
	if((fifosR = open(argv[3], O_RDONLY)) < 0){  
		perror("error opening reading pipe in worker\n");
		return -1;
	}

	if((fifosW = open(argv[2], O_WRONLY)) < 0){  
		perror("error opening writing pipe in worker\n");
		return -1;
	}
	// // error checking for fcntl 
	// if (fcntl(fifosR[i], F_SETFL, O_NONBLOCK) < 0) {
	// 	perror("fnctl fifoR");
	// 	return -1; 
	// }
	// printf("KALISPERA\n");
	char *stats = malloc(sizeof(char) *1000000);
	int diseaseHashNum = 7;
	int countryHashNum = 7;
	int bucketSize = 70;
	//initialize hash tables 
	bucket **diseaseHashTable = malloc(sizeof(bucket *) * diseaseHashNum);
	bucket **countryHashTable = malloc(sizeof(bucket *) * countryHashNum);

	for (int i = 0; i < diseaseHashNum; i++)
		diseaseHashTable[i] = NULL;

	for (int i = 0; i < countryHashNum; i++)
		countryHashTable[i] = NULL;
	
	int capacity = (bucketSize - sizeof(struct b*) - sizeof(int) )/ sizeof(bucket_entry);
	// printf("capacity %d\n", capacity);

	list_node *head = NULL; // head of list 

	// int bytesread = 0;


	int size = 0;

	/* blocking pipe*/
	while(1){
	  	int retVal = read(fifosR, &size, sizeof(int));
	  	// printf("retVal %d\n", retVal);

	    if (retVal == -1)
	    {
	    	continue;
	    }
		// bytesread = read(fifosR, buffer, size);
		read(fifosR, buffer, size);
		// printf("bytesread %d\n", bytesread);
	   
	    buffer[size] = '\0';
	    // printf("Received: %s  ----  bytesread: %d\n", buffer, bytesread);
	    // sleep(3);
	    if (strcmp(buffer, "EOM") == 0)
	    {
	    	// printf("EOM Received\n");
	    	break;
	    }
	    // printf("Received: %s  ----  bytesread: %d -----pid: %d\n", buffer, bytesread, getpid());
		append_path_list(&path_head, buffer);

		// sleep(1);
	}

	char *serverIP;
	int serverPort = 0;
	size = 0;
	read(fifosR, &size, sizeof(int));
  	// printf("retVal %d\n", retVal);
	serverIP = malloc(size * sizeof(char) + 1);
	// bytesread = read(fifosR, serverIP, size);
	read(fifosR, serverIP, size);
	// printf("bytesread %d\n", bytesread);
    serverIP[size] = '\0';
   	read(fifosR, &serverPort, sizeof(int));
    // printf("Received: %s --- %d ----  bytesread: %d\n", serverIP, serverPort, bytesread);


    //listening port 

    int port, sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen = sizeof(struct sockaddr_in);
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    //struct hostent *rem;
   
    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(INADDR_ANY);      /* The given port */

    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");

    getsockname(sock, (struct sockaddr *)&server, &clientlen); // pairnei ti proti diathesimi port apo to sistima kai ti vazei sto server
    port = ntohs(server.sin_port);
    /* Listen for connections */
    if (listen(sock, 5) < 0) perror_exit("listen");








    sprintf(stats, "w@%d$", port);

    // printf("stats--- %s\n", stats);
	paths_list_node * cur = path_head;
	while(cur != NULL){
		// printf("cur->path %s\n", cur->path);
		char *stat = dirCounty(cur->path, &head, diseaseHashTable, countryHashTable, diseaseHashNum, countryHashNum, capacity, serverIP, serverPort);
		sprintf(stats,"%s%s", stats, stat);
		cur = cur->next;
	}


	// printf("STATS APO WORKER %d\n", getpid());
	// printf("%s\n", stats);
	// printf("telos %d\n", getpid());
	//write to server all stats at once


	paths_list_node *cur1 = path_head;
	while(cur1 != NULL){
		// printf("%s\n", cur1->path);
		char * token = strtok(cur1->path, "/");
		token = strtok(NULL, " ");
		sprintf(stats, "%s#%s", stats, token);
		cur1 = cur1->next;
	}

	sprintf(stats, "%s&", stats);
	// printf("stats-->%s\n", stats);

	connecttoserver(serverIP, serverPort, stats);
	

	printf("Listening for connections to port %d, %d\n", port, server.sin_addr.s_addr);
    while (1) {
        /* accept connection */
    	if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");

    	// convert to client ip
		struct sockaddr_in *addr_in = (struct sockaddr_in *)clientptr;
		char *s = inet_ntoa(addr_in->sin_addr);


    	printf("Accepted connection from %s\n", s);

        char buff[500];
        memset(buff, 0, sizeof(buff));
        read(newsock, buff, 500);

        printf("%s\n", buff);
        worker_response(buff, diseaseHashTable, diseaseHashNum, countryHashTable, countryHashNum, head, path_head, newsock);
    	// close(newsock); 
    }







	/* blocking pipe*/


    // print_list(head);
   	// print_ranges(diseaseHashTable, diseaseHashNum);
	// print_path_list(path_head);

	// close(fifofds);
	free_path_list(path_head);
	printf("Worker with pid= %d exiting...\n", getpid());
	return 0;	
}
