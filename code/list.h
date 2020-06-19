#ifndef LIST
#define LIST

#include "structs.h"
#include "functions.h"
#include "functions1.h"
#include "structs.h"
#include "heap.h"
#include "whoServer.h"


// typedef struct li_node {
// 	entry *data;
// 	struct li_node *next;
// } list_node;

list_node * sortedInsert(list_node **head, entry *new_entry);
list_node* append(list_node** head, entry* new_entry);

list_node* search(list_node* head, char *id);
void print_list(list_node *head);
void free_list(list_node *head);



//list for paths
paths_list_node* append_path_list(paths_list_node **head, char *new_entry);
void print_path_list(paths_list_node *head);
void print_path_list_with_pid(paths_list_node *head, int pid);
void free_path_list(paths_list_node *head);


//list for worker_info
worker_info* append_worker_list(worker_info **head, char *IP, int fd, int countriesCounter, char **countries);
void print_worker_list(worker_info *head);
void free_worker_list(worker_info *head);


#endif
