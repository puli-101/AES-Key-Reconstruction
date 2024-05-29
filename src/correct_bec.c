#include <stdio.h>
#include <stdlib.h>
#include "aes.h"
#include "util.h"
#define MAX_SIZE 8192

//sample execution : ./bin/correct_bec samples/aes-128-bin_erasure.txt -v=false

char grid[15][8][8][4]; 
int key_length;
static int VERBOSE = 1;

void usage(char* name) {
    fprintf(stderr,"Usage : %s <filename> [options]\n", name);
    fprintf(stderr,"Where <filename> contains a corrupted AES key schedule that went through the binary erasure channel\n");
    fprintf(stderr,"For file formatting details, execute first ./bin/corruption <src_file> <probability> <channel_type> [options] or see ./samples/bin_erasure.txt\n");
    fprintf(stderr,"\nOptions :\n-verbose : v=true | v=false\n");
    exit(EXIT_FAILURE);
}

void parse_input(char*, int);

int main(int argc, char** argv) {
    if (argc < 2) {
        usage(argv[0]);
    }
    char raw[MAX_SIZE];
    char *file = argv[1];
    int size = extract_text(file,raw);

    //on determine la taille de la clef a partir de la taille du fichier
    if (size < 1820) {
        key_length = 128;
    } else if (size > 4900) {
        key_length = 256;
    } else {
        key_length = 192;
    }

    if (VERBOSE) {
        printf("Extracted schedule :\n%sTotal length : %d\nType : AES-%d\n",raw,size,key_length);
    }

    parse_input(raw, size);

    

    return EXIT_SUCCESS;
}

void parse_input(char* raw, int size) {
    int a,b,c,d;
    a = b = c = d = 0;
    for(int i = 0; i < size - 1; i++) {
        if (raw[i] == '\n') {
            a++;
            b = c = d = 0;
        } else if (raw[i] == ' ' && raw[i+1] == ' ') {
            b++;
            i++;
            c = d = 0;
        } else if (raw[i] == ' ') {
            c++;
            d = 0;
        }else {
            grid[a][b][c][d] = raw[i];
            d++;
        }
    }
}