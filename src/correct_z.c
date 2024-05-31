#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "util.h"
#define MAX_SIZE 8192

//sample execution : ./bin/correct_z samples/aes-128-z_channel.txt -v=false

uint32_t grid[15][8];       //representation d'un key schedule
int key_length;             //taille de la clef aes (128,192,256)
list* unresolved;           //liste de cordonnees des bits inconnus
int nb_unknown;             //nombre de bits inconnus
int rows, columns;

void usage(char* name) {
    print_color(stderr,"Input Formatting Error","red",'\n');
    print_color(stderr, "Usage :","yellow",' ');
    fprintf(stderr,"\033[0;36m%s <filename> <probability> [options]\033[0m\n", name);
    fprintf(stderr,"Where\n- \033[0;36m<filename>\033[0m contains a corrupted AES key schedule that went through the z-channel\n");
    fprintf(stderr, "- \033[0;36m<probability>\033[0m indicates the bit decay probability\n");
    fprintf(stderr,"For file formatting details, execute first ./bin/corruption <src_file> <probability> z-channel -v=false or see ./samples/aes-128-z_channel.txt\n");
    print_color(stderr, "Options :","yellow",'\n');
    fprintf(stderr,"- \033[0;36mverbose\033[0m : -v=true | -v=false\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        usage(argv[0]);
    }

    
    

    return EXIT_SUCCESS;
}