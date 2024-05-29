#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#define MAX_SIZE 2048

//sample execution : ./bin/corruption samples/aes-128-schedule.txt 0.25 bin-erasure -v=true

static int VERBOSE = 1;

typedef enum {
  Z_CHANNEL,
  BIN_ERASURE,
  BIN_SYMM
} channel; 

void usage(char* name) {
    fprintf(stderr,"Usage : %s <filename> <probability> <type>\n\n",name);
    fprintf(stderr,"Where\n- 'filename' contains a -textual- AES key schedule formated as a grid (see ./bin/keygen 's output)\n");
    fprintf(stderr, "- 'probability' indicates the error probability of every bit and is represented as a float between 0 and 1\n");
    fprintf(stderr,"- 'type' indicates the type of channel the key schedule will be subjected to. ");
    fprintf(stderr, "Accepted types include z-channel, bin-symm, and bin-erasure which correspond to the Z-channel, binary symmetric channel, and the binary erasure channel\n");
    exit(EXIT_FAILURE);
}

//returns a random double between 0 and 1
double randf() {
    return (double)rand()/(double)(RAND_MAX);
}

//extracts key schedule from text file
void extract_text(char*, char*);

//given a hexadecimal digit 'hex' and a type of  communication channel
//randomly determines what will happen to each of the four digits of 'hex'
//returns number of bits flipped / lost
int handle_quartet(char, double, channel);

int main(int argc, char** argv) {
    srand(time(NULL));

    char* name, *endPtr;
    double p;
    channel t;
    char schedule[MAX_SIZE];
    int len;

    if (argc < 4) {
        usage(argv[0]);
    }

    //traitement des options
    for (int i = 4; i < argc; i++) {
        //check if verbose disabled
        if (!strcmp(argv[i],"-v=false")) {
            VERBOSE = 0;
        }
    }
    
    //traitement des options
    name = argv[1];
    p = strtod( argv[2], &endPtr ); 
    if (endPtr == argv[2]) {
        fprintf(stderr, "Error : Bad string format: %s \n", endPtr);
        usage(argv[0]);
    } 
    //-- filtrage des types
    if (!strcmp(argv[3],"z-channel")) {
        t = Z_CHANNEL;
    } else if (!strcmp(argv[3],"bin-symm")) {
        t = BIN_SYMM;
    } else if (!strcmp(argv[3],"bin-erasure")) {
        t = BIN_ERASURE;
    } else {
        fprintf(stderr,"Error : Unknown type %s \n",argv[3]);
        usage(argv[0]);
    }

    //extraction de key schedule
    extract_text(name, schedule);
    len = strlen(schedule);
    if (VERBOSE)
        printf("Extracted key schedule : \n%s\n",schedule);
    
    //modification du key schedule
    int losses = 0;
    int actual_size = 0;
    for (int i = 0; i < len; i++) {
        if (schedule[i] == ' ' || schedule[i] == '\n') {
            printf("%c",schedule[i]);
            continue;
        }
        losses += handle_quartet(schedule[i], p, t);
        actual_size += 4;
    }

    if (VERBOSE)
        printf("Loss percentage : %.3f %%\n", (float)(100 * losses) / (float)actual_size);
    
    return EXIT_SUCCESS;
}

//extracts key schedule from text file
void extract_text(char* file, char* output) {
    int length;
    FILE* f = fopen(file, "r");
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    fread (output, 1, length, f);
    fclose(f);
}

//given a hexadecimal digit 'hex' and a type of  communication channel
//randomly determines what will happen to each of the four digits of 'hex'
//returns number of bits flipped / lost
int handle_quartet(char hex, double pr, channel t) {
    char hexa[] = {hex, '\0'};
    uint8_t modif = (uint8_t)strtol(hexa, NULL, 16); //bitwise operations are done as ints not chars
    int loss_counter = 0;

    switch(t) { 
        case BIN_ERASURE:
        //binary erasure case
        for (int i = 0; i < 4 ; i ++) {
            if (randf() <= pr) {
                printf("X");
                loss_counter++;
            } else if (modif & (1 << (3-i))) {
                printf("1");
            } else {
                printf("0");
            }
        }
        printf(" ");
        break;

        case BIN_SYMM:
        //binary symmetric case
        for (int i = 0; i < 4 ; i ++) {
            if (randf() <= pr) {
                //inversion d'un bit avec xor
                modif ^= 1 << i;
                loss_counter++;
            } 
        }
        printf("%x",modif);
        break;

        case Z_CHANNEL:
        //Z channel case
        for (int i = 0; i < 4 ; i ++) {
            //on teste si le bit est a 1
            if ((modif & (1 << i)) && randf() <= pr) {
                //inversion d'un bit avec xor
                modif ^= 1 << i;
                loss_counter++;
            } 
        }
        printf("%x",modif);
    }
    return loss_counter;
}