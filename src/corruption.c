#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 1024

//sample run : ./bin/corruption output 0.5 bin-erasure

static int VERBOSE = 1;

typedef enum {
  Z_CHANNEL,
  BIN_ERASURE,
  BIN_SYMM
} type; 

void usage(char* name) {
    fprintf(stderr,"Usage : %s <filename> <probability> <type>\n\n",name);
    fprintf(stderr,"Where\n- 'filename' contains a -textual- AES key schedule formated as a grid (see ./bin/keygen 's output)\n");
    fprintf(stderr, "- 'probability' indicates the error probability of every bit and is represented as a float between 0 and 1\n");
    fprintf(stderr,"- 'type' indicates the type of channel the key schedule will be subjected to. ");
    fprintf(stderr, "Accepted types include z-channel, bin-symm, and bin-erasure which correspond to the Z-channel, binary symmetric channel, and the binary erasure channel\n");
    exit(EXIT_FAILURE);
}

//returns a random double between 0 and 1
double random() {
    return (double)rand()/(double)(RAND_MAX);
}

//extracts key schedule from text file
void extract_text(char*, char*);

int main(int argc, char** argv) {
    srand(time(NULL));

    char* name, *endPtr;
    double p;
    type t;
    char schedule[MAX_SIZE];
    int len;

    if (argc != 4) {
        usage(argv[0]);
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
    for (int i = 0; i < len; i++) {
        if (schedule[i] == ' ' || schedule[i] == '\n') continue;
        handle_quartet(schedule[i], p, t);
    }

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