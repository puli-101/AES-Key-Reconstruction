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

#endif