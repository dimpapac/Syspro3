#include "list.h"
#include "functions.h"
#include "structs.h"
#include "heap.h"


list_node * sortedInsert(list_node **head, entry *new_entry){
	list_node *current;

	list_node* new_node = malloc(sizeof(list_node));
	new_node->data = new_entry;
	new_node->next = NULL;

	// if (*head == NULL || (*head)->data->date >= new_entry->data->date) 
	if (*head == NULL || earlier( &((*head)->data->entryDate), &(new_node->data->entryDate)) == -1) // >
    { 
        new_node->next = *head; 
        *head = new_node; 
    } 
    else if (earlier( &((*head)->data->entryDate), &(new_node->data->entryDate)) == 0)
    {
    	new_node->next = (*head)->next;
    	(*head)->next = new_node;
    }
    else
    { 
        current = *head; 
        // while (current->next!=NULL && current->next->data < new_node->data) 
        while (current->next!=NULL && earlier(&(current->next->data->entryDate), &(new_node->data->entryDate)) != -1) //<=
        { 
            current = current->next; 
        } 
        new_node->next = current->next; 
        current->next = new_node; 
    } 

   	return new_node;
}


list_node* append(list_node **head, entry *new_entry){

	list_node* new_node = malloc(sizeof(list_node));
	list_node* last = *head;
	new_node->data = new_entry;
	new_node->next = NULL;

	if (*head == NULL) //list is empty 
	{
		*head = new_node; 
	}
	else
	{
		//traverse till the last node 
		while (last->next != NULL)
			last = last->next; 

		last->next = new_node; 		
	}

	return new_node;
}

//return 1 if id found in list
//else 0
list_node *search(list_node* head, char *id) 
{ 
    list_node* current = head;  // Initialize current 
    while (current != NULL) 
    { 
        if (strcmp(current->data->recordID, id) == 0)        	
            return current; 
        current = current->next; 
    } 
    return NULL; 
}


void print_list(list_node *head){
	printf("--------------PRINTING LIST--------------\n");
	// if (head == NULL)
	// 	return;

	while(head != NULL){
		// printf("kalispera apo print list\n");
		print_entry(head->data);
		head = head->next;
	}
}

void free_node(list_node *node){
	free(node->data->recordID);
	free(node->data->patientFirstName);
	free(node->data->patientLastName);
	free(node->data->diseaseID);
	free(node->data->country);
	free(node->data);
	free(node);
}

void free_list(list_node *head){
	list_node *tmp = NULL;
	while (head != NULL){
		tmp = head;
		head = head->next;
		free_node(tmp);
		// free(tmp);
	}
}









///////list for paths


paths_list_node* append_path_list(paths_list_node **head, char *new_entry){

	paths_list_node *new_node = malloc(sizeof(paths_list_node));
	new_node->path = malloc((strlen(new_entry) + 1) * sizeof(char));
	strcpy(new_node->path, new_entry);
	new_node->next = NULL;

	paths_list_node* last = *head;
	if (*head == NULL) //list is empty 
	{
		*head = new_node; 
	}
	else
	{
		//traverse till the last node 
		while (last->next != NULL)
			last = last->next; 

		last->next = new_node; 		
	}

	return new_node;
}



void print_path_list(paths_list_node *head){
	printf("--------------PRINTING LIST--------------\n");
	// if (head == NULL)
	// 	return;

	while(head != NULL){
		// printf("kalispera apo print list\n");
		printf("path---> %s \n", head->path);
		head = head->next;
	}
}

void print_path_list_with_pid(paths_list_node *head, int pid){
	// printf("--------------PRINTING LIST--------------\n");
	// if (head == NULL)
	// 	return;

	while(head != NULL){
		// printf("kalispera apo print list\n");
		printf("%s %d\n", head->path, pid);
		head = head->next;
	}
}


void free_path_list(paths_list_node *head){
	paths_list_node *tmp = NULL;
	while (head != NULL){
		tmp = head;
		head = head->next;
		free(tmp->path);
		free(tmp);
	}
}





//list for worker_info



worker_info* append_worker_list(worker_info **head, char *IP, int fd, int countriesCounter, char **countries){

	worker_info *new_node = malloc(sizeof(worker_info));
	
	new_node->fd = fd;
	new_node->IP = malloc((strlen(IP) + 1) * sizeof(char));
	strcpy(new_node->IP, IP);
	new_node->counter = countriesCounter;
    new_node->countries = malloc(sizeof(char *) * countriesCounter);
    for (int i = 0; i < countriesCounter; i++)
	{
		new_node->countries[i] = malloc((strlen(countries[i])+1) * sizeof(char));
        strcpy(new_node->countries[i], countries[i]);
	}
	new_node->next = NULL;

	worker_info* last = *head;
	if (*head == NULL) //list is empty 
	{
		*head = new_node; 
	}
	else
	{
		//traverse till the last node 
		while (last->next != NULL)
			last = last->next; 

		last->next = new_node; 		
	}

	return new_node;
}



void print_worker_list(worker_info *head){
	printf("--------------PRINTING WORKER LIST--------------\n");
	// if (head == NULL)
	// 	return;

	while(head != NULL){
		// printf("kalispera apo print list\n");
		printf("fd---> %d\n", head->fd);
		printf("IP---> %s \n", head->IP);
		printf("countriesCounter %d\n", head->counter);
		for (int i = 0; i < head->counter; ++i)
		{
			printf("%s\n", head->countries[i]);
		}
		head = head->next;
	}
}


void free_worker_list(worker_info *head){
	worker_info *tmp = NULL;
	while (head != NULL){
		tmp = head;
		head = head->next;
		free(tmp->IP);
		free(tmp);
	}
}
