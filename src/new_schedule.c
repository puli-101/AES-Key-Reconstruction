//Code qui genere la nouvelle representation d'aes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "aes.h"
#include "util.h"
#define NB_BYTES 16
#define ROUNDS 11
#define BLOCKS 4

static uint8_t key[NB_BYTES];
static uint8_t s[ROUNDS][NB_BYTES];

//https://eprint.iacr.org/2020/1253.pdf page 7

void usage(char* name) {
    print_header(name,"[Key] [-v=false]");
    fprintf(stderr,"Where 'Key' represents an AES-128 key in hexadecimal and -v=false disables verbose\n");
    exit(EXIT_FAILURE);
}

void print_key() {
    for (int i = 0; i < NB_BYTES; i++) {
        if (i%4 == 0) printf(" ");
        printf("%02x",key[i]);
    }
}

void init() {
    s[0][0] = key[15];
    s[0][4] = key[14];
    s[0][8] = key[13];
    s[0][12] = key[12];

    s[0][1] = key[14] ^ key[10] ^ key[6] ^ key[2];
    s[0][5] = key[13] ^ key[9] ^ key[5] ^ key[1];
    s[0][9] = key[12] ^ key[8] ^ key[4] ^ key[0];
    s[0][13] = key[15] ^ key[11] ^ key[7] ^ key[3];

    s[0][2] = key[13] ^ key[5];
    s[0][6] = key[12] ^ key[4];
    s[0][10] = key[15] ^ key[7];
    s[0][14] = key[14] ^ key[6];

    s[0][3] = key[12] ^ key[8];
    s[0][7] = key[15] ^ key[11];
    s[0][11] = key[14] ^ key[10];
    s[0][15] = key[13] ^ key[9];
}

void calc_schedule() {
    init();
    int index;

    for (int i = 1; i < ROUNDS; i++) {
        for (int j = 0; j < BLOCKS; j++) {
            index = (13 + 4 * j)%16;
            s[i][4 * j] = s[i-1][index] ^ sbox[s[i][index-1]];
        }
        s[i][12] ^= rcon[i];

        for (int j = 0; j < BLOCKS; j++) {
            index = (14 + 4 * j)%16;
            s[i][4 * j + 1] = s[i-1][index];
            index = (15 + 4 * j)%16;
            s[i][4 * j + 2] = s[i-1][index];
            index = (12 + 4 * j)%16;
            s[i][4 * j + 3] = s[i-1][index];
        }
    }
}

void print_new_schedule(uint8_t s[11][16], uint8_t *key) {
    if (key != NULL && VERBOSE) {
        print_color(stdout,"Original Key","yellow",'\n');
        print_key();
        print_color(stdout, "\n\nAlternative Schedule Representation","yellow",'\n');
    }
    for (int i = 0; i < ROUNDS; i++) {
        for (int j = 0; j < NB_BYTES; j++) {
            if (j%4 == 0) printf(" ");
            printf("%02x",s[i][j]);
        }
        printf("\n");
    }
}



void parse_input(char* skey) {
    char word[9];
    word[3] = '\0';
    for (int i = 0; i < 32; i+=8) {
        for (int j = 0; j < 2; j++)
            word[j] = skey[i + j];
        key[i/2] = (int)strtol(word, NULL, 16);
    }

    if (VERBOSE) {
        print_color(stdout,"Parsed input key","yellow",'\n');
        print_key();
        printf("\n");
    }
}

int main(int argc, char** argv) {
    int random = 1;
    char* skey;

    for (int i = 1; i < argc; i++) {
        if (!strcmp("-v=false",argv[i]))
            VERBOSE = 0;
        else if (strlen(argv[i]) == 32) {
            random = 0;
            skey = argv[1];
        }
    }
    
    if (!random) {
        //if a key was given as an input then we extract it
        parse_input(skey);
    } else {
        //si on donne pas une clef en entree alors 
        //on genere une clef aleatoire de 128 bits
        if (VERBOSE)
            print_color(stdout,"Randomly generated 128-bit key :","yellow",'\n'); 
        srand(time(NULL));  
        for (int i = 0; i < NB_BYTES; i++) {
            key[i] = (uint8_t)rand();
        }
        if (VERBOSE) {
            print_key();
            printf("\n\n");
        }
    }

    calc_schedule();
    print_new_schedule(s,key);
    return 0;
}