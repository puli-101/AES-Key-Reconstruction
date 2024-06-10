#ifndef _UTIL_H_
#define _UTIL_H_

#include "aes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

//Various functions used by almost all sourcefiles

extern int VERBOSE;
extern int SHORTENED;

typedef enum {
  Z_CHANNEL,
  BIN_ERASURE,
  BIN_SYMM
} channel; 

//functions to print in color
void set_color(FILE*, char*);
void print_color(FILE*, char*, char*,char);
char* get_color(char* color);

//Function used when execution arguments are wrong
//Given the name of the executable and the list of arguments
//prints the format to be used to properly execute file
void print_header(char*,char*);

//prints a progress bar on screen
void print_progress(double percentage);

//calculates combinations n choose k
static inline double choose(int n, int k) {
  return (tgamma(n + 1)/(tgamma(k + 1) * (tgamma(n - k + 1))));
}

//returns a random double between 0 and 1
static inline double randf() {
    return (double)rand()/(double)(RAND_MAX);
}

//custom definition of absolute value
//it'd be simpler to just negate the bit that corresponds to 
//sign in the IEEE 754 representation of double
static inline double abs_double(double x) {
    if (x < 0)
        return -x;
    return x;
}

//Given an array of 4 bytes, determines the corresponing value
//of a 32 bit type with all 4 of those bytes
static inline uint32_t byteArrayInto32_t(uint8_t array[4]) {
    uint32_t res = 0;
    for (int i = 0; i < 4; i++) {
        res |= ((uint32_t)array[i]) << (i * 8);
    }
    return res;
}

//extracts all bytes of a text and stores it in a buffer
//returns the size of the textfile
int extract_text(char*, char*);

//Given the raw contents of a file and its size
//initializes the extracted key schedule into grid
void parse_input(char* raw, int size, uint8_t grid[ROUNDS][NB_BYTES]);

//xor of a and b where a and b are characters in {'0','1'}
char ascii_xor(char a, char b);

//returns the nth byte of a 4-byte word (starting from 0)
uint8_t get_byte_from_word(uint32_t,int);

#endif