#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "util.h"
#define MAX_SIZE 8192

//sample execution : ./bin/correct_z samples/aes-128-z_channel.txt 0.0625 -v=false

uint32_t grid[15][4];       //representation d'un key schedule
int key_length;             //taille de la clef aes (128,192,256)
list* unresolved;           //liste de cordonnees des bits inconnus
int nb_unknown;             //nombre de bits inconnus
int rounds;
float prob;

void usage(char* name) {
    print_header(name,"<filename> <probability> [options]");
    fprintf(stderr,"Where\n- \033[0;36m<filename>\033[0m contains a corrupted AES key schedule that went through the z-channel\n");
    fprintf(stderr, "- \033[0;36m<probability>\033[0m indicates the bit decay probability as a floating point number between 0 and 1\n");
    fprintf(stderr,"For file formatting details, execute first ./bin/corruption <src_file> <probability> z-channel -v=false or see ./samples/aes-128-z_channel.txt . ");
    fprintf(stderr,"In general, the file must consist of a set of 32-bit-long hexadecimal numbers separated by a single space ' '. Line jumps will be ignored.\n");
    print_color(stderr, "Options :","yellow",'\n');
    fprintf(stderr,"- \033[0;36mverbose\033[0m : -v=true | -v=false\n");
    exit(EXIT_FAILURE);
}

void parse_input(char*,int);
void determine_ambiguity();

int main(int argc, char** argv) {
    if (argc < 3) {
        usage(argv[0]);
    }

    for (int i = 3; i < argc; i++) {
        if (!strcmp(argv[i], "-v=false"))
            VERBOSE = 0;
    }
    
    char raw[MAX_SIZE];
    char *file = argv[1];
    int size = extract_text(file,raw);
    prob = atof(argv[2]);

    parse_input(raw,size);
    if (VERBOSE) {
        print_color(stdout,"Parsed input : ","yellow",'\n');
        print_schedule(grid,rounds);
    }
    
    determine_ambiguity();
    if (VERBOSE) {
        print_color(stdout,"- Number of ambiguous bits :","yellow",' ');
        printf("\t%d\n",nb_unknown);
        print_color(stdout, "- Percentage of ambiguous bits :","yellow",' ');
        printf("\t%f %%\n",(float)(100 * nb_unknown)/((float)128 * rounds));
        print_color(stdout, "- Expected number of flipped bits :","yellow",' ');
        printf("\t%d\n", (int)((float)nb_unknown * prob));
    }
    free_list(&unresolved);
    return EXIT_SUCCESS;
}

void parse_input(char* raw, int size) {
    char* beginning = raw, *end;
    int x = 0, y = 0;

    for (int i = 0; i < size - 1; i++) {
        if (raw[i] == '\n') continue;
        if (raw[i] == ' ') {
            raw[i] = '\0';
            grid[x][y] = strtol(beginning, &end, 16); 
            if (end == beginning)
                usage("./executable_name");
            y++;
            if (y >= 4) {
                y = 0;
                x++;
            }
            raw[i] = ' ';
            beginning = raw + i + 1;
            if (raw[i+1] == '\n')
                beginning++;
        } 
    }
    rounds = x;
}

//Calculates the number of bits set to 0 
//(which potentially decayed from bits set to 1)
void determine_ambiguity() {
    for (int i = 0; i < rounds; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 32; k++) {
                if (!((1 << k) & grid[i][j])) {
                    int coord[] = {i,j,k};
                    insert(&unresolved, coord, 3);
                    nb_unknown++;
                }
            }
        }
    }
}