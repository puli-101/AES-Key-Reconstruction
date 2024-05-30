#ifndef __UTIL_H__
#define __UTIL_H__

#include "aes.h"
#include <stdio.h>
#include <stdlib.h>

extern int VERBOSE;

typedef enum {
  Z_CHANNEL,
  BIN_ERASURE,
  BIN_SYMM
} channel; 

//returns a random double between 0 and 1
static inline double randf() {
    return (double)rand()/(double)(RAND_MAX);
}

//extracts all bytes of a text and stores it in a buffer
//returns the size of the textfile
int extract_text(char*, char*);

//xor of a and b where a and b are characters in {'0','1'}
char ascii_xor(char a, char b);

//DOUBLE LINKED LIST FUNCTIONS
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