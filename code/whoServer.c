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

#include "whoServer.h"

void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int bufferSize;
int num_of_items = 15;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t list_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_nonempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_nonfull = PTHREAD_COND_INITIALIZER;
pool_t *pool;


void print_pool(){
	printf("pool->start %d\n", pool->start);
	printf("pool->end %d\n", pool->end);
	printf("pool->count %d\n", pool->count);
	for (int i = 0; i < pool->count; ++i)
	{
		printf("fd: %d\n",pool->client_array[i].fd);
		printf("IP: %s\n",pool->client_array[i].IP);
	}
}

void initialize() {
	pool = malloc(sizeof(pool_t));
	pool->start = 0;
	pool->end = -1;
	pool->count = 0;
	pool->client_array = malloc(sizeof(Client_info) * bufferSize);
}

void place(Client_info client_info) {
	pthread_mutex_lock(&mtx);
	while (pool->count >= bufferSize) {
		printf(">> Found Buffer Full \n");
		pthread_cond_wait(&cond_nonfull, &mtx);
		}
	pool->end = (pool->end + 1) % bufferSize;
	pool->client_array[pool->end] = client_info;
	pool->count++;
	pthread_cond_signal(&cond_nonempty);
	pthread_mutex_unlock(&mtx);
}

Client_info obtain() {
	Client_info client_info;
	printf("what\n");
	pthread_mutex_lock(&mtx);
	while (pool->count <= 0) {
		printf(">> Found Buffer Empty \n");
		pthread_cond_wait(&cond_nonempty, &mtx);
	}
	client_info = pool->client_array[pool->start];
	pool->start = (pool->start + 1) % bufferSize;
	pool->count--;
	pthread_cond_signal(&cond_nonfull);
	pthread_mutex_unlock(&mtx);
	return client_info;
}

// void * producer(void * ptr)
// {
// 	while (num_of_items > 0) {
// 		place(&pool, num_of_items);
// 		printf("producer: %d\n", num_of_items);
// 		num_of_items--;
// 		pthread_cond_signal(&cond_nonempty);
// 		usleep(1000);
// 		}
// 	pthread_exit(0);
// }

void * consumer()
{
	Client_info client_info;
	while(1){
		printf("elamou\n");
		client_info = obtain();
		/*
			1. read apo client_info.fd enan int (sizeof(int))
				if int = 0 
					handle worker
				else 
					handle client
			read apo client_info.fd enan terastio array kai strtok gia tis ipolipes plirofories 
		*/
	}
}

/* handle worker
	read port number (se enan int) kai to vazw stin lista gia tous workers(global) 
	read ta statistika apo ton worker 
	read tis xores se lista opos eixe o aggregatos gia na kaleitai to idio cli 
	
	lock(list_mutex)
	insert worker stin lista
	print(statistics)
	unlock(list_mutex)


*/

int main(int argc, char *argv[])
{

	if (argc != 9)
	{
		printf("Usage: ./whoServer –p queryPortNum –s statisticsPortNum –w numThreads -b bufferSize\n");
		return -1;
	}

	int numThreads, queryPortNum, statisticsPortNum;
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

	// printf("karaseg\n");
	// // threads
	// pthread_t thr[numThreads];
	// int err, status;
	// for (int i = 0; i < numThreads; i++)
	// {
	// 	if ((err = pthread_create(&thr[i], NULL, consumer, NULL))) { /* New thread */
	// 		perror("pthread_create");
	// 		exit(1); 
	// 	}
	// }




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

    
    initialize(&pool);
    Client_info client_info;
    printf("Listening for connections to port %d, %d\n", port, server.sin_addr.s_addr);
    while (1) {
        /* accept connection */
    	if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");

    	// convert to client ip
		struct sockaddr_in *addr_in = (struct sockaddr_in *)clientptr;
		char *s = inet_ntoa(addr_in->sin_addr);

    	client_info.fd = newsock;
    	client_info.IP = malloc(sizeof(char)* (strlen(s) + 1));
    	strcpy(client_info.IP, s);
    	place(client_info);
    	print_pool();

    	printf("Accepted connection from %s\n", s);

    	Client_info toread = obtain();
    	printf("from obtain %d %s\n", toread.fd, toread.IP);
    	print_pool();

        char buff[1000000];
        memset(buff, 0, sizeof(buff));
        read(newsock, buff, 1000000);

        // printf("%s\n", buff);

    	close(newsock); /* parent closes socket to client */
    }

    



	return 0;

}