#ifndef __WHOSERVER__
#define __WHOSERVER__



#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


typedef struct wi{
	int counter; 
	char **countries; //malloc(sizeof(counter)) diplo malloc
	int fd; 
	char *IP;
	struct wi *next;
} worker_info;

typedef struct {
	int fd;
	char *IP;
} Client_info;


typedef struct {
	Client_info *client_array;
	int start;
	int end;
	int count;
} pool_t;

void initialize(); 
void place(Client_info client_info); 
Client_info obtain(); 
void * producer(void * ptr);
void * consumer();

#endif