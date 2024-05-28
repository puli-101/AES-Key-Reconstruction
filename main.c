#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "aes.h"
#define VERBOSE 1

static uint32_t key[8];
static uint32_t exp_key[15][8];
static int key_size = 128; 
static int rounds = 10;

//parse string representant des octets -> int
void parse_input(char* skey) {
    char byte[9];
    byte[8] = '\0';
    for (int i = 0; i < key_size; i+=8) {
        for (int j = 0; j < 8; j++)
            byte[j] = skey[i + j];
        key[i/8] = (int)strtol(byte, NULL, 16);
    }

    for (int i = 0; VERBOSE && i < key_size/32; i++)
        printf("%x ",key[i]);
    if (VERBOSE)
        printf("\n");
}

void usage(char* name) {
    fprintf(stderr,"Usage : %s [KEY]\n", name);
    fprintf(stderr,"Where KEY is a 32, 48 or 64 character long string representing a 128, 192 or 256 bit AES key respectively coded in hexadecimal\n");
    exit(-1);
}

void calc_key_schedule() {
    int N = key_size/32;
    for (int i = 0; i < rounds + 1; i ++ ) {
        for (int j = 0; j < N; j++) {
            if (i == 0) {
                exp_key[i][j] = key[j];
            } else if (j == 0) {
                exp_key[i][j] = exp_key[i-1][j] ^ sub(rot(exp_key[i-1][N-1])) ^ rcon[0];
            } else if (N > 6 && i%N == 4) {
                exp_key[i][j] = exp_key[i-1][j] ^ sub(exp_key[i-1][N-1]);
            } else {
                exp_key[i][j] = exp_key[i-1][j] ^ exp_key[i-1][N-1];
            }
        } 
    }
}

void print_key_schedule() {
    //rounds = 10, 12 ou 14
    for (int i = 0; i < rounds + 1; i ++ ) {
        for (int j = 0; j < key_size/32; j++) {
            printf("%x ",exp_key[i][j]);
        } 
        printf("\n");
    }
}

int main(int argc, char** argv) {
    int len;
    if (argc == 2) {
        //m-a-j de la taille de la clef
        len = strlen(argv[1]);
        key_size = len * 4;
        rounds = 10 + (key_size - 128) / 32;

        if (len != 64 && len != 48 && len != 32) 
            usage(argv[0]);
        parse_input(argv[1]);
    } else {
        //si on donne pas une clef en entree alors 
        //on genere une clef aleatoire de 128 bits
        if (VERBOSE)
            printf("Randomly generated 128-bit key : ");
        srand(time(NULL));  
        for (int i = 0; i < 4; i++) {
            key[i] = rand();
            if (VERBOSE)
                printf("%x ",key[i]);
        }
        if (VERBOSE)
            printf("\n");
    }
    calc_key_schedule();

    if (VERBOSE)
        printf("Derived key scheduled :\n");
    print_key_schedule();

    return EXIT_SUCCESS;
}