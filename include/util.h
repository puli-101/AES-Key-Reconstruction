#ifndef __UTIL_H__
#define __UTIL_H__

#include "aes.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

//Various functions used throughout all sourcefiles

extern int VERBOSE;

typedef enum {
  Z_CHANNEL,
  BIN_ERASURE,
  BIN_SYMM
} channel; 

//functions to print in color
void set_color(FILE*, char*);
void print_color(FILE*, char*, char*,char);
char* get_color(char* color);

void print_header(char*,char*);

void print_progress(double percentage);

//calculates combinations n choose k
static inline double choose(int n, int k) {
  return (tgamma(n + 1)/(tgamma(k + 1) * (tgamma(n - k + 1))));
}

//returns a random double between 0 and 1
static inline double randf() {
    return (double)rand()/(double)(RAND_MAX);
}

static inline double abs_double(double x) {
    if (x < 0)
        return -x;
    return x;
}

//extracts all bytes of a text and stores it in a buffer
//returns the size of the textfile
int extract_text(char*, char*);

//xor of a and b where a and b are characters in {'0','1'}
char ascii_xor(char a, char b);

//returns the nth byte of a 4-byte word (starting from 0)
uint8_t get_byte_from_word(uint32_t,int);

#endif