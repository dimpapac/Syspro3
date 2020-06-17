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
#include "list.h"
// #include "rbt.h"
// #include "heap.h"


void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}




int main(int argc, char *argv[])
{

	if (argc != 9)
	{
		printf("Usage: ./whoServer –p queryPortNum –s statisticsPortNum –w numThreads -b bufferSize\n");
		return -1;
	}

	int bufferSize, numThreads, queryPortNum, statisticsPortNum;
	int flags = 4;
	for (int i = 1; i < argc; i+=2)
	{
		if (strcmp(argv[i], "-p") == 0)
		{
			queryPortNum = atoi(argv[i+1]);
			flags--;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{	
			statisticsPortNum = atoi(argv[i+1]);
			flags--;
		}
		else if (strcmp(argv[i], "-w") == 0)
		{	
			numThreads = atoi(argv[i+1]);
			flags--;
		}
		else if (strcmp(argv[i], "-b") == 0)
		{	
			bufferSize = atoi(argv[i+1]);
			flags--;
		}
	}

	// printf("flags %d\n", flags);
	if(flags){
		printf("wrong flags\n");
		return -1;
	}

	// printf("%d %d %d %d\n", queryPortNum, statisticsPortNum, numThreads, bufferSize);

	

	int port, sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen = sizeof(struct sockaddr_in);
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    //struct hostent *rem;
   
    port = statisticsPortNum;

    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    // server.sin_port = htons(port);      /* The given port */

    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");

    getsockname(sock, (struct sockaddr *)&server, &clientlen); // pairnei ti proti diathesimi port apo to sistima kai ti vazei sto server
    port = ntohs(server.sin_port);
    /* Listen for connections */
    if (listen(sock, 5) < 0) perror_exit("listen");
    printf("Listening for connections to port %d, %d\n", port, server.sin_addr.s_addr);
    while (1) {
        /* accept connection */
    	if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");
    	// convert to client ip
		struct sockaddr_in *addr_in = (struct sockaddr_in *)clientptr;
		char *s = inet_ntoa(addr_in->sin_addr);

    	printf("Accepted connection from %s\n", s);

        char buff[1000];
        memset(buff, 0, sizeof(buff));
        read(newsock, buff, 1000);

        printf("%s\n", buff);

    	close(newsock); /* parent closes socket to client */
    }



	return 0;

}