#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include "structs.h"
// #include "functions1.h"
// #include "list.h"
// #include "heap.h"


void listCountries(paths_list_node **list_head, int *pids, int numWorkers);
void cli(paths_list_node **list_head, int *pids, int numWorkers, int*, int*);
int dirCounty(char *buffer, list_node **head, bucket **diseaseHashTable, bucket **countryHashTable, int, int, int, int);
void worker_response(char *buffer, bucket **diseaseHashTable, int diseaseHashNum, bucket **countryHashTable, \
	int countryHashNum, list_node *head, paths_list_node *path_head, int fifosW);


void print_ranges(bucket **HashTable, int HashNum);

#endif
