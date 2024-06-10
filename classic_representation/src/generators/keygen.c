#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "aes.h"
#include "util.h"

static uint32_t key[8];
static uint32_t exp_key[15][4];
static int key_size = 128; 
static int rounds = 11;

//Given 'skey' a string representing a k-bit long number in hexadecimal
//it casts every 8 consecutive characters into a 32-bit word
//and stores it in 'key'
void parse_input_key(char* skey) {
    char word[9];
    word[8] = '\0';
    for (int i = 0; i < key_size; i+=8) {
        for (int j = 0; j < 8; j++)
            word[j] = skey[i + j];
        key[i/8] = (int)strtol(word, NULL, 16);
    }

    for (int i = 0; VERBOSE && i < key_size/32; i++)
        printf("%08x ",key[i]);
    if (VERBOSE)
        printf("\n");
}

void usage(char* name) {
    print_header(name,"[KEY] [-v=false | -v=true]");
    fprintf(stderr,"Where \033[0;36mKEY\033[0m is a 32, 48 or 64 character long string representing a 128, 192 or 256 bit AES key coded in hexadecimal\n");
    print_color(stderr,"Options :","yellow", '\n');
    fprintf(stderr,"-v=false indicates verbose disabled\n");
    exit(-1);
}

//Classic AES key schedule algorithm 
void calc_key_schedule() {
    int N = key_size/32;
    for (int i = 0; i < 4 * rounds; i ++ ) {
        if (i < N) {
            exp_key[0][i] = key[i];
        } else if (i%N == 0) {
            exp_key[0][i] = exp_key[0][i-N] ^ sub(rot(exp_key[0][i-1])) ^ (((uint32_t)rcon[i/N]) << 24);
        } else if (N > 6 && i%N == 4) {
            exp_key[0][i] = exp_key[0][i-N] ^ sub(exp_key[0][i-1]);
        } else {
            exp_key[0][i] = exp_key[0][i-N] ^ exp_key[0][i-1];
        }
        
    }
}

int main(int argc, char** argv) {
    int len;
    int random = 1;
    char* skey;

    //traitement des options
    for (int i = 1; i < argc; i++) {
        len = strlen(argv[i]);
        //check if verbose disabled
        if (!strcmp(argv[i],"-v=false")) {
            VERBOSE = 0;
        }
        //m-a-j de la taille de la clef
        else if (len == 64 || len == 48 || len == 32) {
            random = 0;
            key_size = len * 4;
            rounds += (key_size - 128) / 32;
            skey = argv[i];
        } 
        //else -> erreur format
        else {
            usage(argv[0]);
        }
    }


    if (!random) {
        //if a key was given as an input then we extract it
        parse_input_key(skey);
    } else {
        //si on donne pas une clef en entree alors 
        //on genere une clef aleatoire de 128 bits
        if (VERBOSE)
            print_color(stdout,"Randomly generated 128-bit key :","yellow",'\n'); 
        srand(time(NULL));  
        for (int i = 0; i < 4; i++) {
            key[i] = rand();
            if (VERBOSE)
                printf("%08x ",key[i]);
        }
        if (VERBOSE)
            printf("\n\n");
    }

    calc_key_schedule();

    if (VERBOSE)
        print_color(stdout,"Derived key scheduled :","yellow",'\n');
    
    print_schedule(exp_key, rounds);

    return EXIT_SUCCESS;
}