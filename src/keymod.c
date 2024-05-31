#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "util.h"
#define MAX_SIZE 2048

//sample execution : ./bin/keymod samples/aes-128-schedule.txt 0.25 bin-erasure -v=true

void usage(char* name) {
    print_header(name, "<filename> <probability> <type> [options]");
    fprintf(stderr,"Where\n- \033[0;36m'filename'\033[0m\t\tcontains a -textual- AES key schedule formated as a grid (see ./bin/keygen 's output)\n");
    fprintf(stderr, "- \033[0;36m'probability'\033[0m \tindicates the error probability of every bit and is represented as a float between 0 and 1\n");
    fprintf(stderr,"- \033[0;36m'type'\033[0m\t\tindicates the type of channel the key schedule will be subjected to. ");
    fprintf(stderr, "\nAccepted types include \033[0;32mz-channel\033[0m, \033[0;32mbin-symm\033[0m, and \033[0;32mbin-erasure\033[0m which correspond to the Z-channel, binary symmetric channel, and the binary erasure channel\n");
    print_color(stderr, "Options :","yellow",'\n');
    fprintf(stderr,"- \033[0;36mverbose\033[0m : -v=true | -v=false\n");
    exit(EXIT_FAILURE);
}

//given a hexadecimal digit 'hex' and a type of  communication channel
//randomly determines what will happen to each of the four digits of 'hex'
//finally, it returns number of bits flipped / lost
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
    if (VERBOSE) {
        print_color(stdout,"Extracted key schedule :","yellow",'\n');
        printf("%s\n\n",schedule);
        print_color(stdout,"Modified key schedule :","yellow",'\n');
    }
    //modification du key schedule
    int losses = 0;
    int actual_size = 0;
    for (int i = 0; i < len; i++) {
        if (schedule[i] == '\0' || schedule[i] == 0) break;
        if (schedule[i] == ' ' || schedule[i] == '\n') {
            printf("%c",schedule[i]);
            continue;
        }
        losses += handle_quartet(schedule[i], p, t);
        actual_size += 4;
    }

    if (VERBOSE) {
        print_color(stdout, "\nLoss percentage :", "yellow", ' ');
        printf("%.3f %%\n", (float)(100 * losses) / (float)actual_size);
    }
    
    return EXIT_SUCCESS;
}

//given a hexadecimal digit 'hex' and a type of  communication channel
//randomly determines what will happen to each of the four digits of 'hex'
//finally, it returns number of bits flipped / lost
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