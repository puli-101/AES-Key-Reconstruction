#ifndef __LIST__H__
#define __LIST__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//DOUBLE LINKED LIST FUNCTIONS
//(idk why I redid it)

typedef struct t_list {
  struct t_list* next;
  struct t_list* prev;
  int size;
  int* data;
} list;

void insert(list**, int*, int);
void delete_elt(list**, list*);
void check(void*);
void free_cell(list*);
int is_empty(list*);
void free_list(list**);
list* getFromIndex(list*, int);
void print_list(list*);

#endif