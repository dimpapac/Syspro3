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
#include <poll.h>



#include "structs.h"
#include "functions.h"
#include "list.h"
// #include "rbt.h"
// #include "heap.h"

#include "whoServer.h"

#define MAX_BUF 1000000
#define MAX_FD 2 

int bufferSize;
int num_of_items = 15;

//mutexes and condition variables
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t list_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_nonempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_nonfull = PTHREAD_COND_INITIALIZER;

//public data structures
pool_t *pool; //circular buffer
worker_info *worker_info_head;


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
	// printf("what\n");
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


void handle_client(char *buffer, int fd){

	printf("%s\n", buffer);
    send_request(worker_info_head, fd, buffer);
}

void * consumer() //thread function
{
	Client_info client_info;
	while(1){
		client_info = obtain();
        char buff[MAX_BUF];
        memset(buff, 0, sizeof(buff));
        read(client_info.fd, buff, MAX_BUF);
        //strtok buff --> w@port$stats#country#country#country
        char *token = strtok(buff, "@");
        if (strcmp(token,"w") == 0 )
        {
        	char *port_s = strtok(NULL, "$");
        	int port = atoi(port_s);
        	char *stats = strtok(NULL, "#");
        	char *countriesString = strtok(NULL, "&");

        	//count countries
		    int countriesCounter = 0;

		    for (int i = 0; i < strlen(countriesString); i++)
		    {
		    	if (countriesString[i] == '#')
		    	{
		    		countriesCounter++;
		    	}
		    }

		    countriesCounter++; // +1 of '#'

        	char **countries = malloc(sizeof(char *) * countriesCounter);

        	int pos = 0;
        	char *country = strtok(countriesString, "#");
        	while(country != NULL){
        		countries[pos] = malloc((strlen(country)+1) * sizeof(char));
        		strcpy(countries[pos], country);
        		pos++;
        		country = strtok(NULL, "#");
        	}

        	for (int i = 0; i < countriesCounter; ++i)
        	{
        		// printf("%s\n", countries[i]);
        	}

	        pthread_mutex_lock(&list_mtx); //lock for global list and print 
        	printf("%s\n", stats);
	        append_worker_list(&worker_info_head, client_info.IP, port, countriesCounter, countries);
	        // printf("thread id: %ld\n", pthread_self() );
	        pthread_mutex_unlock(&list_mtx); //unlock
        	// print_worker_list(worker_info_head);
        }
        else
        {
        	// buff[MAX_BUF-1] = '\0';
        	handle_client(token, client_info.fd);

        }

    	close(client_info.fd); /* server closes socket to client */

	}
}

void reset_socket_info(Socket_info *socket_info, int stats_sock, int query_sock){
	socket_info[0].fd = stats_sock;
	socket_info[1].fd = query_sock;
	for (int i = 0; i < MAX_FD; i++){
		socket_info[i].flag = 0;
	}
}

//return 1 if fds == 1 
int checkFlags(Socket_info *socket_info){
	for (int i = 0; i < MAX_FD; ++i){
		if (socket_info[i].flag != 0){
			return 0;
		}
	}

	return 1;
}




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


	worker_info_head = NULL;

	initialize(&pool);

	// threads
	pthread_t thr[numThreads];
	int err;
	for (int i = 0; i < numThreads; i++)
	{
		if ((err = pthread_create(&thr[i], NULL, consumer, NULL))) { /* New thread */
			perror("pthread_create");
			exit(1); 
		}
	}



	int stats_sock, query_sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen = sizeof(struct sockaddr_in);
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    //struct hostent *rem;
   

    /* Create socket */
    if ((stats_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 || (query_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(statisticsPortNum);      /* The given port */

    /* Bind socket to address */
    if (bind(stats_sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");

    server.sin_port = htons(queryPortNum);
     /* Bind socket to address */
    if (bind(query_sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");

    getsockname(stats_sock, (struct sockaddr *)&server, &clientlen); 
    statisticsPortNum = ntohs(server.sin_port);

    getsockname(query_sock, (struct sockaddr *)&server, &clientlen); 
    queryPortNum = ntohs(server.sin_port);
    /* Listen for connections */
    if (listen(stats_sock, 15) < 0) perror_exit("listen");

    if (listen(query_sock, 15) < 0) perror_exit("listen");

    
    
    Client_info client_info;
    printf("Listening for connections to statisticsPortNum %d, %d\n", statisticsPortNum, server.sin_addr.s_addr);
    printf("Listening for connections to queryPortNum %d, %d\n", queryPortNum, server.sin_addr.s_addr);

    Socket_info socket_info[MAX_FD];
    while (1){
    	struct pollfd pollfds[MAX_FD];
    	reset_socket_info(socket_info, stats_sock, query_sock);

    	while(checkFlags(socket_info)){
	        for (int i = 0; i < MAX_FD; i++)
	        {
	            pollfds[i].fd = socket_info[i].fd;
	            pollfds[i].events = POLLIN;
	        }

	        poll(pollfds, MAX_FD, -1);
	        for (int i = 0; i < MAX_FD; i++)
	        {                
	            if((pollfds[i].revents & POLLIN))
	            {
	                if(pollfds[i].fd == socket_info[i].fd){
	                	    /* accept connection */
						if ((newsock = accept(socket_info[i].fd, clientptr, &clientlen)) < 0) perror_exit("accept");

						// convert to client ip
						struct sockaddr_in *addr_in = (struct sockaddr_in *)clientptr;
						char *s = inet_ntoa(addr_in->sin_addr);

						client_info.fd = newsock;
						client_info.IP = malloc(sizeof(char)* (strlen(s) + 1));
						strcpy(client_info.IP, s);
						place(client_info);
						// print_pool();
						socket_info[i].flag = 1;
						printf("Accepted connection from %s\n", s);
						// print_worker_list(worker_info_head);
	                }
	            }
	            else if((pollfds[i].revents & POLLHUP)){
                    socket_info[i].flag = 0;
	            }
	        }        	
    	}
    }




	return 0;

}