#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <signal.h>
// #include <errno.h>

// #include "structs.h"
// #include "rbt.h"
#include "functions.h"
#include "functions1.h"
// #include "heap.h"



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

int pflag;
int ptotal;
int psuccess;
int pfail;
int ctotal;
int csuccess;
int cfail;


void listCountries(paths_list_node **list_head, int *pids, int numWorkers){

	for (int i = 0; i < numWorkers; i++){
		if (list_head[i] != NULL){
			// printf("okrrr %s %d\n", list_head[i]->path, pids[i]);
			print_path_list_with_pid(list_head[i], pids[i]);
		}
	}
}


int find_worker_with_country(paths_list_node **list_head, int numWorkers, char *country){
	for (int i = 0; i < numWorkers; i++)
	{
		paths_list_node * cur = list_head[i];
		while(cur != NULL){
			if (strcmp(cur->path, country) == 0)
			{
				return i;
			}
			cur = cur->next;
		}
	}
	return -1;
}

void write_line(paths_list_node **list_head, int *pids, int numWorkers, int *fifosR, int *fifosW, char *send_line, int flag){
	if (flag == 0) //send to every child
	{
		for (int i = 0; i < numWorkers; i++)
		{
			int size = (int) strlen(send_line) + 1;
            // printf("sizeOfMessage dA  -- %d\n", size);
           	//write size of message 
           	if (write(fifosW[i], &size, sizeof(int)) == -1){ 
				perror("write");
				// return -1;
			}
			// sleep(1);

			// // write message
            if (write(fifosW[i], send_line, size) == -1){ 
				perror("write");
				// return -1;
			}
		}
	}
	else if(flag == 1) //send to 1 child
	{
		// printf("/diseaseFrequency write line \n");
		char *temp_line = malloc((strlen(send_line) + 1) * sizeof(char));
		strcpy(temp_line, send_line);
		char *token = strtok(temp_line," ");
		token = strtok(NULL, " ");
		token = strtok(NULL, " ");
		token = strtok(NULL, " ");
		token = strtok(NULL, " ");
		int position = find_worker_with_country(list_head, numWorkers, token);
		if (position == -1)
		{
			printf("country not exists\n");
			free(temp_line);
			return;
		}
		else{
			// printf("POSITION %d\n", position);
			int size = (int) strlen(send_line) + 1;
            // printf("sizeOfMessage dA  -- %d\n", size);
           	//write size of message 
           	if (write(fifosW[position], &size, sizeof(int)) == -1){ 
				perror("write");
				// return -1;
			}
			// sleep(1);

			// // write message
            if (write(fifosW[position], send_line, size) == -1){ 
				perror("write");
				// return -1;
			}
		}
		free(temp_line);
	}

}



void read_answer(paths_list_node **list_head, int *pids, int numWorkers, int *fifosR, int *fifosW, char *send_line, int flag){
	if (flag == 0) // no country
	{
		int counter = 0;
		for (int i = 0; i < numWorkers; i++) //read from all workers
		{
			int returnValue = 0; 
			while (read(fifosR[i], &returnValue, sizeof(int)) < 0){ 
		   		// if (errno == EAGAIN) { 
			    //     printf("(pipe empty)\n"); 
			    //     sleep(1);
			    //     // break; 
			    // }
			    // printf("PAMELIGO\n");
			}
	    	counter += returnValue;
		}
		// printf("COUNTER IN READ_ANSER: %d\n", counter);
		printf("%d\n", counter);
	}
	else if(flag == 1) //country 
	{
		// printf("/diseaseFrequency write line \n");
		char *temp_line = malloc((strlen(send_line) + 1) * sizeof(char));
		strcpy(temp_line, send_line);
		char *token = strtok(temp_line," "); // npa
		token = strtok(NULL, " "); //disease
		token = strtok(NULL," "); //date1
		token = strtok(NULL," "); //date2
		token = strtok(NULL," "); // country
		int position = find_worker_with_country(list_head, numWorkers, token);
		if (position == -1)
		{
			printf("country not exists\n");
		}
		else{
			// printf("POSITION %d\n", position);
			int returnValue = 0; 
			while (read(fifosR[position], &returnValue, sizeof(int)) < 0){ 
		   		// if (errno == EAGAIN) { 
			    //     printf("(pipe empty)\n"); 
			    //     sleep(1);
			    //     // break; 
			    // }
			    // printf("PAMELIGO\n");
			}
			// printf("returnValue IN READ_ANSER: %d\n", returnValue);
			printf("%d\n", returnValue);
		}
		
		free(temp_line);
	}

}




void read_answer_string(paths_list_node **list_head, int *pids, int numWorkers, int *fifosR, int *fifosW, char *send_line, int flag){
	if (flag == 0) // no country
	{
		for (int i = 0; i < numWorkers; i++) //read from all workers
		{
			while(1){
				int size = 0; 
				while (read(fifosR[i], &size, sizeof(int)) < 0){ 
			   		// if (errno == EAGAIN) { 
				    //     printf("(pipe empty)\n"); 
				    //     sleep(1);
				    //     // break; 
				    // }
				    // printf("PAMELIGO\n");
				}

				if (size == 0)
				{
					printf("no outbreaks read_answer_string\n");
				}

				if (size == -13)
		    	{
		    		break;
		    	}

				char *buffer = malloc(sizeof(char) * (size + 1));
				while (read(fifosR[i], buffer, size) < 0){ 
			   		// if (errno == EAGAIN) { 
				    //     printf("(pipe empty)\n"); 
				    //     sleep(1);
				    //     // break; 
				    // }
				    // printf("PAMELIGO\n");
				}
				buffer[size] = '\0';
				printf("%s\n", buffer);
				free(buffer);
			}

		}
		// printf("COUNTER IN READ_ANSER: %d\n", counter);
		// printf("%d\n", counter);
	}
	else if(flag == 1) //country 
	{
		// printf("/diseaseFrequency write line \n");
		char *temp_line = malloc((strlen(send_line) + 1) * sizeof(char));
		strcpy(temp_line, send_line);
		char *token = strtok(temp_line," "); // npa
		token = strtok(NULL, " "); //disease
		token = strtok(NULL," "); //date1
		token = strtok(NULL," "); //date2
		token = strtok(NULL," "); // country
		int position = find_worker_with_country(list_head, numWorkers, token);
		if (position == -1)
		{
			printf("country not exists\n");
		}
		else{
			// printf("POSITION %d\n", position);
			int size = 0; 
			while (read(fifosR[position], &size, sizeof(int)) < 0){ 
		   		// if (errno == EAGAIN) { 
			    //     printf("(pipe empty)\n"); 
			    //     sleep(1);
			    //     // break; 
			    // }
			    // printf("PAMELIGO\n");
			}

			if (size == 0)
			{
				printf("no outbreaks read_answer_string\n");
			}

			char *buffer = malloc(sizeof(char) * (size + 1));
			while (read(fifosR[position], buffer, size) < 0){ 
		   		// if (errno == EAGAIN) { 
			    //     printf("(pipe empty)\n"); 
			    //     sleep(1);
			    //     // break; 
			    // }
			    // printf("PAMELIGO\n");
			}
			buffer[size] = '\0';
			printf("%s\n", buffer);
			free(buffer);
		}
		free(temp_line);
	}

}



void cli(paths_list_node **list_head, int *pids, int numWorkers, int *fifosR, int *fifosW){

	ssize_t lineSize;

	//command line interface 
	size_t n;
	char *command = NULL;
	do {
		if (command != NULL)
			free(command);
		command = NULL;
		printf(">");
		if (pflag == 1)
		{
			break;
		}
		// printf("FLAG META DO %d\n", pflag);
		lineSize = getline(&command, &n, stdin); //get line from stdin
		command[lineSize - 1] = '\0';
		// printf("COMMAND: %s\n", command); 
/*1*/	if (strncmp(command, "/listCountries", strlen("/listCountries")) == 0 || strncmp(command, "lc", strlen("lc")) == 0) {
			// printf("/listCountries\n");
			
			listCountries(list_head, pids, numWorkers);
			ptotal++;
			psuccess++;

/*2*/	} else if (strncmp(command, "/diseaseFrequency", strlen("/diseaseFrequency")) == 0 || strncmp(command, "df", strlen("df")) == 0) {
			// printf("/diseaseFrequency\n");
			char *send_line = malloc((strlen(command) + 1) * sizeof(char));
			strcpy(send_line, command);
			int flag = 0;

			char *token = strtok(command," ");
			if (token == NULL) continue;
    		// printf("%s\n", token);
    		char *virusName = strtok(NULL, " ");
    		if (virusName == NULL) {
    			printf("Usage:  /diseaseFrequency virusName date1 date2 [country] \n");
    			ptotal++;
    			pfail++;
    			free(send_line);
    			continue;
			}// printf("%s\n", virusName);

			char *date1 = strtok(NULL," ");
			if (date1 == NULL){
				printf("Usage:  /diseaseFrequency virusName date1 date2 [country] \n");
    			ptotal++;
    			pfail++;
    			free(send_line);
				continue;
			}

			char *date2 = strtok(NULL," ");
			if (date2 == NULL){
				printf("Usage: /diseaseFrequency virusName date1 date2 [country] \n");
				ptotal++;
				pfail++;
				free(send_line);
				continue;
			}

			char *country = strtok(NULL," ");
			if (country == NULL)
			{
				// printf("no country \n");
				// printf("send_line  %s\n", send_line);
				write_line(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);
				read_answer(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);
			}
			else
			{
				flag = 1;
				// printf("country: %s\n", country);

				write_line(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);
				read_answer(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);

			}

			ptotal++;
			psuccess++;
			free(send_line);

/*3*/	} else if (strncmp(command, "/topk-AgeRanges", strlen("/topk-AgeRanges")) == 0 || strncmp(command, "topk", strlen("topk")) == 0) {
			// printf("/topk-AgeRanges\n");
			printf("Not implemented\n");
			continue;
			char *token = strtok(command," "); //topk
			if (token == NULL) continue;
    		char *ks = strtok(NULL, " ");
    		if (ks == NULL){
    			printf("Usage: /topk-AgeRanges k country disease date1 date2\n");
    			ptotal++;
    			pfail++;
    			continue;
    		}
    		int k = atoi(ks);
    		if (k == 0)
    		{
    			printf("k must be a number\n");
    			ptotal++;
    			pfail++;
    			continue;
    		}
    		char *country = strtok(NULL, " ");
    		if (country == NULL){
    			printf("yUsage: /topk-AgeRanges k country disease date1 date2\n");
    			ptotal++;
    			pfail++;
    			continue;
    		}
    		char *disease = strtok(NULL, " ");
    		if (disease == NULL){
    			printf("Usage: /topk-AgeRanges k country disease date1 date2\n");
    			ptotal++;
    			pfail++;
    			continue;
    		}
    		char *date1 = strtok(NULL, " ");
			if (date1 == NULL) //no dates given
			{
				// printf("no dates\n");
				// date nodate; //NULL 
				// topk(countryHashTable, countryHashNum, k, country, 0, nodate, nodate);
			}
			else
			{
				// printf("%s\n", date1);
				char *date2 = strtok(NULL," ");
				if (date2 == NULL){
					printf("you have to give two dates\n");
					ptotal++;
    				pfail++;
					continue;
				}
				else {
					// date idate1, idate2;
					// if (charToDate(date1, &idate1) != 0 || charToDate(date2, &idate2) != 0 || earlier(&idate2, &idate1) == 1)
					// {
					// 	printf("wrong dates\n");
					// 	continue;
					// }
					
					// topk(countryHashTable, countryHashNum, k, country, 2, idate1, idate2);
				}
			}

			ptotal++;
			psuccess++;

/*4*/	} else if (strncmp(command, "/searchPatientRecord", strlen("/searchPatientRecord")) == 0 || strncmp(command, "spr", strlen("spr")) == 0) {
			// printf("searchPatientRecord\n");
			char *send_line = malloc((strlen(command) + 1) * sizeof(char));
			strcpy(send_line, command);
			char *recordID = strtok(command," ");
			int flag = 0;
			// char *exitDate;
			recordID = strtok(NULL, " ");
			if (recordID == NULL){
				printf("no recordID given\n");
				free(recordID);
				ptotal++;
    			pfail++;
    			free(send_line);
				continue;
			}
			write_line(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);

			int counter = 0;
			for (int i = 0; i < numWorkers; i++) //read from all workers
			{
				
				int size = 0; 
				while (read(fifosR[i], &size, sizeof(int)) < 0){ 
			   		// if (errno == EAGAIN) { 
				    //     printf("(pipe empty)\n"); 
				    //     sleep(1);
				    //     // break; 
				    // }
				    // printf("PAMELIGO\n");
				}

				if (size == 0)
				{	
					counter++;
					continue;
				}

				char *buffer = malloc(sizeof(char) * (size + 1));
				while (read(fifosR[i], buffer, size) < 0){ 
			   		// if (errno == EAGAIN) { 
				    //     printf("(pipe empty)\n"); 
				    //     sleep(1);
				    //     // break; 
				    // }
				    // printf("PAMELIGO\n");
				}
				buffer[size] = '\0';
				printf("%s\n", buffer);
			

				free(buffer);

			}

			if (counter == numWorkers)
			{
				printf("recordID not found\n");
			}

			ptotal++;
			psuccess++;
			free(send_line);

/*5*/	} else if (strncmp(command, "/numPatientAdmissions", strlen("/numPatientAdmissions")) == 0 || strncmp(command, "npa", strlen("npa")) == 0) {
			// printf("/numPatientAdmissions\n");
			char *send_line = malloc((strlen(command) + 1) * sizeof(char));
			strcpy(send_line, command);
			char *token = strtok(command," ");
			int flag = 0;

			if (token == NULL) continue;
    		// printf("%s\n", token);
    		char *virusName = strtok(NULL, " ");
    		if (virusName == NULL) {
    			printf("Usage: /numPatientAdmissions disease date1 date2 [country] \n");
    			ptotal++;
    			pfail++;
    			free(send_line);
    			continue;
			}// printf("%s\n", virusName);

			char *date1 = strtok(NULL," ");
			if (date1 == NULL){
				printf("Usage: /numPatientAdmissions disease date1 date2 [country] \n");
				ptotal++;
    			pfail++;
    			free(send_line);
				continue;
			}

			char *date2 = strtok(NULL," ");
			if (date2 == NULL){
				printf("Usage: /numPatientAdmissions disease date1 date2 [country] \n");
				ptotal++;
    			pfail++;
    			free(send_line);
				continue;
			}

			char *country = strtok(NULL," ");
			if (country == NULL)
			{
				// printf("no country \n");
				// frequency(diseaseHashTable, diseaseHashNum, date1, date2, virusName);
				write_line(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);
				read_answer_string(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);
				
			}
			else
			{
				flag = 1;
				// printf("country: %s\n", country);
				// frequencyWithCountry(countryHashTable, countryHashNum, date1, date2, virusName, country);
				write_line(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);
				read_answer_string(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);

			}

			ptotal++;
			psuccess++;
			free(send_line);

/*6*/	} else if (strncmp(command, "/numPatientDischarges", strlen("/numPatientDischarges")) == 0 || strncmp(command, "npd", strlen("npd")) == 0) {
			// printf("/numPatientDischarges\n");
			char *send_line = malloc((strlen(command) + 1) * sizeof(char));
			strcpy(send_line, command);
			char *token = strtok(command," ");
			int flag = 0;
			if (token == NULL) continue;
    		// printf("%s\n", token);
    		char *disease = strtok(NULL, " ");
    		if (disease == NULL) {
    			printf("Usage: /numPatientDischarges disease date1 date2 [country] \n");
    			ptotal++;
    			pfail++;
    			continue;
    			free(send_line);
			}// printf("%s\n", disease);

			char *date1 = strtok(NULL," ");
			if (date1 == NULL){
				printf("Usage: /numPatientDischarges disease date1 date2 [country] \n");
				ptotal++;
    			pfail++;
				continue;
				free(send_line);
			}

			char *date2 = strtok(NULL," ");
			if (date2 == NULL){
				printf("Usage: /numPatientDischarges disease date1 date2 [country] \n");
				ptotal++;
    			pfail++;
				continue;
				free(send_line);
			}

			char *country = strtok(NULL," ");
			if (country == NULL)
			{
				// printf("no country \n");
				// frequency(diseaseHashTable, diseaseHashNum, date1, date2, disease);
				write_line(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);
				read_answer_string(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);

			}
			else
			{
				flag = 1;
				// printf("country: %s\n", country);
				// frequencyWithCountry(countryHashTable, countryHashNum, date1, date2, disease, country);
				write_line(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);
				read_answer_string(list_head, pids, numWorkers, fifosR, fifosW, send_line, flag);

			}	
			
			ptotal++;
			psuccess++;
			free(send_line);

		} else if (strcmp(command, "/exit\0") != 0){
			printf("Wrong command try again\n");
			ptotal++;
			pfail++;
		}
		// printf("FLAG PRIN WHILE %d\n", pflag);


	} while(strcmp(command, "/exit\0")!= 0 && pflag != 1 );
	printf("exiting\n");
	if (command != NULL)
		free(command);

	for (int i = 0; i < numWorkers; i++)
	{
		kill(pids[i], SIGKILL);
	}
}




void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}


void connecttoserver(char* serveraddress, int port, char* command){

	char buf[1256];
	int sock,length;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;


	/* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	perror_exit("socket");
	/* Find server address */
    if ((rem = gethostbyname(serveraddress)) == NULL) {	
	   herror("gethostbyname"); exit(1);
    }

    server.sin_family = AF_INET;       /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);         /* Server port */
	if (connect(sock, serverptr, sizeof(server)) < 0)
   		perror_exit("connect");
	//printf("Connecting to %s port %d\n", argv[1], port);
	if(write(sock, command, strlen(command)) < 0)
		perror_exit("write");
    if ((length=read(sock, buf, 1256)) < 0)
    	perror_exit("read");    
    	   	
 	buf[length]=0;
	// printf("%ssdfgdsfgdfgsdf\n",buf);close(sock); 


}



char * write_stats(bucket **HashTable,  int HashNum, char *date, char *country, char *serveraddress, int port){
	char *string = malloc(sizeof(char) * 1000);

	sprintf(string, "%s", date);
	sprintf(string, "%s\n%s", string, country);

	// printf("string->%s\n", string);

	// printf("date: %s ---- country: %s\n", date, country);
	// printf("%d\n", HashNum);
	for (int i = 0; i < HashNum; i++)
	{
		// printf("%p\n", HashTable[i]);
		if (HashTable[i] != NULL)
		{
			// printf("cur entries: %d for bucket No: %d\n", HashTable[i]->currentNumberOfEntries, i);
			for (int j = 0; j < HashTable[i]->currentNumberOfEntries; j++)
			{
			 	sprintf(string, "%s\n%s\n", string, HashTable[i]->entries[j].nameOfdiseaseORc);
			 	sprintf(string, "%sAge range 0-20 years: %d cases\n", string, HashTable[i]->entries[j].range.counter020);
			 	// HashTable[i]->entries[j].range.counter020 = 0;
			 	sprintf(string, "%sAge range 21-40 years: %d cases\n", string, HashTable[i]->entries[j].range.counter2140);
			 	// HashTable[i]->entries[j].range.counter2140 = 0;
			 	sprintf(string, "%sAge range 41-60 years: %d cases\n", string, HashTable[i]->entries[j].range.counter4160);
			 	// HashTable[i]->entries[j].range.counter4160 = 0;
			 	sprintf(string, "%sAge range 60+ years: %d cases\n", string, HashTable[i]->entries[j].range.counter60);
			 	// HashTable[i]->entries[j].range.counter60 = 0;
			} 
			bucket * last_bucket = HashTable[i]->next;
			while (last_bucket != NULL){
				for (int k = 0; k < last_bucket->currentNumberOfEntries; k++)
				{
			 		sprintf(string, "%s\n%s\n", string, last_bucket->entries[k].nameOfdiseaseORc);
				 	sprintf(string, "%sAge range 0-20 years: %d cases\n", string, last_bucket->entries[k].range.counter020);
				 	// last_bucket->entries[k].range.counter020 = 0;
				 	sprintf(string, "%sAge range 21-40 years: %d cases\n", string, last_bucket->entries[k].range.counter2140);
				 	// last_bucket->entries[k].range.counter2140 = 0;
				 	sprintf(string, "%sAge range 41-60 years: %d cases\n", string, last_bucket->entries[k].range.counter4160);
				 	// last_bucket->entries[k].range.counter4160 = 0;
				 	sprintf(string, "%sAge range 60+ years: %d cases\n", string, last_bucket->entries[k].range.counter60);
				 	// last_bucket->entries[k].range.counter60 = 0;
				}
				last_bucket = last_bucket->next; 
			}
			// printf("\n");
		}
	}

	
	//write to fifoW
	// printf("%s\n", string);
	// int size = (int) strlen(string) + 1;
	// // printf("STRLEN STRING %d\n", (int) strlen(string));
	// if (write(fifosW, &size, sizeof(int)) == -1){ 
	// 	perror("write");
	// 	// return -1;
	// }


	// // printf("STRLEN STRING %d\n", (int) strlen(string));
	// if (write(fifosW, string, size) == -1){ 
	// 	perror("write");
	// 	// return -1;
	// }

	// connecttoserver(serveraddress, port, string);


	// strcpy(string, " ");


	//reset counters to 0
	for (int i = 0; i < HashNum; i++)
	{
		// printf("%p\n", HashTable[i]);
		if (HashTable[i] != NULL)
		{
			// printf("cur entries: %d for bucket No: %d\n", HashTable[i]->currentNumberOfEntries, i);
			for (int j = 0; j < HashTable[i]->currentNumberOfEntries; j++)
			{
			 	HashTable[i]->entries[j].range.counter020 = 0;
			 	HashTable[i]->entries[j].range.counter2140 = 0;
			 	HashTable[i]->entries[j].range.counter4160 = 0;
			 	HashTable[i]->entries[j].range.counter60 = 0;
			} 
			bucket * last_bucket = HashTable[i]->next;
			while (last_bucket != NULL){
				for (int k = 0; k < last_bucket->currentNumberOfEntries; k++)
				{
				 	last_bucket->entries[k].range.counter020 = 0;
				 	last_bucket->entries[k].range.counter2140 = 0;
				 	last_bucket->entries[k].range.counter4160 = 0;
				 	last_bucket->entries[k].range.counter60 = 0;
				}
				last_bucket = last_bucket->next; 
			}
			// printf("\n");
		}
	}

	return string;
}






char *dirCounty(char *countryDir, list_node **head, bucket **diseaseHashTable, bucket **countryHashTable, int diseaseHashNum, int countryHashNum, int capacity, char *serverIP, int port){

	// printf("dirCounty------->%s\n", countryDir);
	// char token[15];
	char *cp = malloc((strlen(countryDir) + 1) * sizeof(char));
	strcpy(cp, countryDir);
	char *country = strtok(cp,"/");
    country = strtok(NULL, " ");
    // printf("%s\n", country);
	/* This is just a sample code, modify it to meet your need */
    struct dirent *de;  // Pointer for directory entry 
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir(countryDir); 
  
    if (dr == NULL){  // opendir returns NULL if couldn't open directory  
        perror("opendir"); 
        // return 0; 
    	return NULL;
    } 
  	char *stats = malloc(sizeof(char) *1000000);
    //de -> directory entry
    while ((de = readdir(dr)) != NULL) {
    	if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) //skip . and ..
    		continue;
    	// printf("countryDir: %s\n", countryDir);
        // printf("de->d_name: %s\n", de->d_name);

    	//read file
		FILE *input;

		char *filepath = malloc((strlen(countryDir) + strlen(de->d_name) + 2) * sizeof(char));
		sprintf(filepath, "%s/%s",countryDir, de->d_name);
		// printf("filepath %s\n", filepath);
		input = fopen(filepath, "r");
		if (input == NULL) {
			perror("Error opening file\n");
		}
		
		char ee[6], name[13], surname[13], disease[15]; 
		int id, age;

		while(fscanf(input, "%d %s %s %s %s %d", &id, ee, name, surname, disease, &age) == 6){
			// printf("id: %d\n", id);
			// printf("ee: %s\n", ee);
			// printf("name: %s\n", name);
			// printf("surname: %s\n", surname);
			// printf("disease: %s\n", disease);
			// printf("age: %d\n", age);


			char line[100];
			sprintf(line, "%d %s %s %s %s %s", id, name, surname, disease, country, de->d_name); 
			// printf("LINE-----   %s\n", line);
			entry* new_entry = line_to_entry(line);
			new_entry->age = age;
			// print_entry(new_entry);

			if (new_entry == NULL)
			{
				printf("problem\n");
				// return -1;
				return NULL;
			}
			// print_entry(new_entry);
				
			if (strcmp(ee,"EXIT") == 0){
				// printf("\n");
				// printf("EXIT RECORD\n");
				// print_entry(new_entry);
				// printf("\n");
				char exitDate[40];
				sprintf(exitDate, "%d-%d-%d", new_entry->entryDate.day, new_entry->entryDate.month, new_entry->entryDate.year);
				// printf("EXIT DATE ISSSSSSS %s\n", exitDate);
				int retVal = recordPatientExit(*head, new_entry->recordID, exitDate);
				if (retVal == 0){
					// printf("Record updated\n");
					ctotal++;
					csuccess++;
				}
				else{
					printf("ERROR\n");
					ctotal++;
					cfail++;
				}

			}
			else if (strcmp(ee, "ENTER") == 0){
				// printf("\n");
				// printf("ENTER RECORD\n");
				// print_entry(new_entry);
				// printf("\n");
				if(search(*head,  new_entry->recordID) != NULL){
					// printf("%s already exists\n", new_entry->recordID); //old output
 					printf("ERROR\n");
 					ctotal++;
					cfail++;
					//free entry
					free(new_entry->recordID);
					free(new_entry->patientFirstName);
					free(new_entry->patientLastName);
					free(new_entry->diseaseID);
					free(new_entry->country);
					free(new_entry);
					// free(line);
					// line = NULL;
					//free data structures
					free_hash(diseaseHashTable, diseaseHashNum);
					free_hash(countryHashTable, countryHashNum);
					free_list(*head);
					//close file
					// fclose(input);
					// free(inputfile);
					// return -1;
					return NULL; 
				}

				//insert patient to a sorted list
				list_node *new_node = sortedInsert(head, new_entry);
				//insert patient to hash tables
				insert_to_hash(diseaseHashTable, diseaseHashNum, new_node->data->diseaseID, new_node, capacity); 
				insert_to_hash(countryHashTable, countryHashNum, new_node->data->country, new_node, capacity);

				ctotal++;
				csuccess++;
			}
			else {
				// return -3;
				return NULL;
			}
		}
		
		char * stat = write_stats(diseaseHashTable, diseaseHashNum, de->d_name, country, serverIP, port);
		sprintf(stats,"%s\n%s", stats, stat);
    }
 	
 // 	//write -15 indication that i finished with stats
 // 	int end = -15;
	// if (write(fifosW, &end, sizeof(int)) == -1){ 
	// 	perror("write");
	// 	// return -1;
	// }

    closedir(dr);


    // print_list(*head);
    // print_hash(diseaseHashTable, diseaseHashNum);


    return stats; 
}




// print entries of bucket	
void print_ranges(bucket **HashTable, int HashNum){
	// printf("%d\n", HashNum);
	for (int i = 0; i < HashNum; i++)
	{
		printf("%p\n", HashTable[i]);
		if (HashTable[i] != NULL)
		{
			printf("cur entries: %d for bucket No: %d\n", HashTable[i]->currentNumberOfEntries, i);
			for (int j = 0; j < HashTable[i]->currentNumberOfEntries; j++)
			{
			 	printf("name of entry %d: %s\n", j, HashTable[i]->entries[j].nameOfdiseaseORc);
			 	printf("Age range 0-20 years: %d cases\n", HashTable[i]->entries[j].range.counter020);
			 	printf("Age range 21-40 years: %d cases\n", HashTable[i]->entries[j].range.counter2140);
			 	printf("Age range 41-60 years: %d cases\n", HashTable[i]->entries[j].range.counter4160);
			 	printf("Age range 60+ years: %d cases\n", HashTable[i]->entries[j].range.counter60);
			} 
			bucket * last_bucket = HashTable[i]->next;
			while (last_bucket != NULL){
				for (int k = 0; k < last_bucket->currentNumberOfEntries; k++)
				{
			 		printf("name of entry %dk: %s\n", k, last_bucket->entries[k].nameOfdiseaseORc);
				 	printf("Age range 0-20 years: %d cases\n", last_bucket->entries[k].range.counter020);
				 	printf("Age range 21-40 years: %d cases\n", last_bucket->entries[k].range.counter2140);
				 	printf("Age range 41-60 years: %d cases\n", last_bucket->entries[k].range.counter4160);
				 	printf("Age range 60+ years: %d cases\n", last_bucket->entries[k].range.counter60);
				}
				last_bucket = last_bucket->next; 
			}
			printf("\n");
		}
	}

}




////////////////////////WORKER/////////////////////////////////

void worker_response(char *buffer, bucket **diseaseHashTable, int diseaseHashNum, bucket **countryHashTable, int countryHashNum, list_node *head, paths_list_node *path_head, int fifosW){
	char *string = malloc((strlen(buffer) + 1) * sizeof(char));
	strcpy(string, buffer);
	char *command = strtok(string," ");	
	// printf("COMMAND IN WORKER RESPONSE  %s\n", command);

	// char *response;
	if (strncmp(command, "/diseaseFrequency", strlen("/diseaseFrequency")) == 0 || strncmp(command, "df", strlen("df")) == 0) {
		// printf("/diseaseFrequency\n");

		// char *token = strtok(string," ");
		char *virusName = strtok(NULL, " ");
		char *date1 = strtok(NULL," ");
		char *date2 = strtok(NULL," ");
		char *country = strtok(NULL," ");
		if (country == NULL)
		{
			// printf("no country \n");
			frequency(diseaseHashTable, diseaseHashNum, date1, date2, virusName, fifosW);
			// printf("send_line  %s\n", send_line);
			// printf("%s %s %s \n", virusName, date1, date2);
		}
		else
		{
			// printf("country: %s\n", country);
			// printf("%s %s %s %s \n", virusName, date1, date2, country);

			frequencyWithCountry(countryHashTable, countryHashNum, date1, date2, virusName, country, fifosW);

		}

		
	// }
// 	} else if (strncmp(command, "/topk-AgeRanges", strlen("/topk-AgeRanges")) == 0 || strncmp(command, "topk", strlen("topk")) == 0) {
// 	printf("/topk-AgeRanges\n");
// 	char *token = strtok(command," "); //topk
// 	if (token == NULL) continue;
// 	char *ks = strtok(NULL, " ");
// 	if (ks == NULL){
// 		printf("Usage: /topk-AgeRanges k country disease date1 date2\n");
// 		continue;
// 	}
// 	int k = atoi(ks);
// 	if (k == 0)
// 	{
// 		printf("k must be a number\n");
// 		continue;
// 	}
// 	char *country = strtok(NULL, " ");
// 	if (country == NULL){
// 		printf("yUsage: /topk-AgeRanges k country disease date1 date2\n");
// 		continue;
// 	}
// 	char *disease = strtok(NULL, " ");
// 	if (disease == NULL){
// 		printf("Usage: /topk-AgeRanges k country disease date1 date2\n");
// 		continue;
// 	}
// 	char *date1 = strtok(NULL, " ");
// 	if (date1 == NULL) //no dates given
// 	{
// 		// printf("no dates\n");
// 		// date nodate; //NULL 
// 		// topk(countryHashTable, countryHashNum, k, country, 0, nodate, nodate);
// 	}
// 	else
// 	{
// 		// printf("%s\n", date1);
// 		char *date2 = strtok(NULL," ");
// 		if (date2 == NULL){
// 			printf("you have to give two dates\n");
// 			continue;
// 		}
// 		else {
// 			// date idate1, idate2;
// 			// if (charToDate(date1, &idate1) != 0 || charToDate(date2, &idate2) != 0 || earlier(&idate2, &idate1) == 1)
// 			// {
// 			// 	printf("wrong dates\n");
// 			// 	continue;
// 			// }
			
// 			// topk(countryHashTable, countryHashNum, k, country, 2, idate1, idate2);
// 		}
// 	}


	} else if (strncmp(command, "/searchPatientRecord", strlen("/searchPatientRecord")) == 0 || strncmp(command, "spr", strlen("spr")) == 0) {
		// printf("searchPatientRecord\n");
		// char *recordID = strtok(command," ");
		// char *exitDate;
		char *recordID = strtok(NULL, " ");
		// printf("recordID in worker: %s\n", recordID);
		list_node *ret = search(head, recordID);
		if (ret == NULL)
		{
			// printf("not found\n");
			int zero = 0;
			if (write(fifosW, &zero, sizeof(int)) == -1){ 
				perror("write");
				// return -1;
			}
			return;
		}
		int message_size = strlen(ret->data->recordID) + strlen(ret->data->patientFirstName) + strlen(ret->data->patientLastName) + \
			strlen(ret->data->diseaseID) + strlen(ret->data->country) + sizeof(int) + 25;
		char *result = malloc(sizeof(char) * (message_size));
		sprintf(result, "%s", ret->data->recordID);
		sprintf(result, "%s %s", result, ret->data->patientFirstName);
		sprintf(result, "%s %s", result, ret->data->patientLastName);
		sprintf(result, "%s %s", result, ret->data->diseaseID);
		sprintf(result, "%s %d", result, ret->data->age);
		sprintf(result, "%s %d", result, ret->data->entryDate.day);
		sprintf(result, "%s-%d", result, ret->data->entryDate.month);
		sprintf(result, "%s-%d", result, ret->data->entryDate.year);
		if(ret->data->exitDate.day == 1 && ret->data->exitDate.month == 1 && ret->data->exitDate.year == 1)
		{
			sprintf(result, "%s  --", result);
		}
		else
		{
			sprintf(result, "%s %d", result, ret->data->exitDate.day);
			sprintf(result, "%s-%d", result, ret->data->exitDate.month);
			sprintf(result, "%s-%d", result, ret->data->exitDate.year);
		}

		// printf("result is %s\n", result);
	
		
		int size = (int) strlen(result) + 1;
		if (write(fifosW, &size, sizeof(int)) == -1){ 
			perror("write");
			// return -1;
		}


		if (write(fifosW, result, size) == -1){ 
			perror("write");
			// return -1;
		}
			
		free(result);

	} else if (strncmp(command, "/numPatientAdmissions", strlen("/numPatientAdmissions")) == 0 || strncmp(command, "npa", strlen("npa")) == 0) {
		// printf("/numPatientAdmissions worker\n");
		// char *token = strtok(command," ");
		// printf("%s\n", token);
		char *virusName = strtok(NULL, " ");
		char *date1 = strtok(NULL," ");
		char *date2 = strtok(NULL," ");
		char *country = strtok(NULL," ");
		// printf("date1 %s, date2 %s\n", date1, date2);
		int flag = 0;
		if (country == NULL)
		{
			// printf("no country \n");
			paths_list_node * cur = path_head;
			while(cur != NULL){
				char tmp[30];
				strcpy(tmp, cur->path);
				char *country = strtok(tmp,"/");
				country = strtok(NULL," ");
				// char *country = path_head->path;
				// printf("COUNTRY numPatientAdmissions-- %s\n", country);
				frequencyWithCountryNPAD(countryHashTable, countryHashNum, date1, date2, virusName, country, fifosW, path_head, flag);
				cur = cur->next;
			}
			
			int size = -13;
			if (write(fifosW, &size, sizeof(int)) == -1){ 
				perror("write");
				// return -1;
			}
		}
		else
		{

			// printf("country: %s\n", country);
			frequencyWithCountryNPAD(countryHashTable, countryHashNum, date1, date2, virusName, country, fifosW, path_head, flag);

		}

	} else if (strncmp(command, "/numPatientDischarges", strlen("/numPatientDischarges")) == 0 || strncmp(command, "npd", strlen("npd")) == 0) {
		// printf("/numPatientDischarges worker\n");
		// char *token = strtok(command," ");
		char *virusName = strtok(NULL, " ");
		char *date1 = strtok(NULL," ");
		char *date2 = strtok(NULL," ");
		char *country = strtok(NULL," ");
		int flag = 1;
		if (country == NULL)
		{
			// printf("no country \n");
			paths_list_node * cur = path_head;
			while(cur != NULL){
				char tmp[30];
				strcpy(tmp, cur->path);
				char *country = strtok(tmp,"/");
				country = strtok(NULL," ");
				// char *country = path_head->path;
				// printf("COUNTRY numPatientAdmissions-- %s\n", country);
				frequencyWithCountryNPAD(countryHashTable, countryHashNum, date1, date2, virusName, country, fifosW, path_head, flag);
				cur = cur->next;
			}
			
			int size = -13;
			if (write(fifosW, &size, sizeof(int)) == -1){ 
				perror("write");
				// return -1;
			}
		}
		else
		{

			// printf("country: %s\n", country);
			frequencyWithCountryNPAD(countryHashTable, countryHashNum, date1, date2, virusName, country, fifosW, path_head, flag);

		}	
		
	}
// 	} else if (strcmp(command, "/exit\0") != 0){
// 		printf("Wrong command try again\n");
// 	}

	// return response;
} 
