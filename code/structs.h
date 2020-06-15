#ifndef STRUCTS
#define STRUCTS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "list.h"
// #include "functions.h"


typedef struct range{
	int counter020;
	int counter2140;
	int counter4160;
	int counter60;
}ranges;



typedef struct {
	int day;
	int month;
	int year;
} date;

typedef struct {
	char *recordID;
	char *patientFirstName; 
	char *patientLastName;
	char *diseaseID;
	char *country;
	int age;
	date entryDate;
	date exitDate;
} entry;

typedef struct li_node {
	entry *data;
	struct li_node *next;
} list_node;

typedef struct paths_li_node {
	char *path;
	struct paths_li_node *next;
} paths_list_node;

typedef struct rbt_node{
	//data of node	
	list_node *listPtr; //pointer to patients-list
	date data_date; //date of node 
	int counter; //number of duplicates 	

	int color; //1 if red 
	struct rbt_node *left, *right, *parent;
} rb_node;

typedef struct {
	char *nameOfdiseaseORc; //field 1
	rb_node *root; //field 2
	ranges range; //field 3
} bucket_entry;

typedef struct b{
	int currentNumberOfEntries;
	bucket_entry *entries; //array of entries

	struct b *next;
} bucket;


typedef struct heapnode{
	int data;
	char *stringData;	

	struct heapnode *left, *right, *parent, *prevTail; 
}heap_node;

typedef struct heap{
	heap_node *root, *tail;
	int size;
}heap;

typedef struct heap_list_node{
	heap_node *hnode;
	struct heap_list_node *next;
}heapListNode;

typedef struct list{
	heapListNode **heapListHead;
}HeapList;


#endif