#include <stdio.h>
#include <stdlib.h>
#include "aes.h"
#include "util.h"
#define MAX_SIZE 8192

//sample execution : ./bin/correct_bec samples/aes-128-bin_erasure.txt -v=false

char grid[15][8][33];       //representation ascii d'un key schedule
int key_length;             //taille de la clef aes (128,192,256)
int unresolved[4000][3];    //cordonnees des bits inconnus
int nb_unknown;             //nombre de bits inconnus
int rows, columns;

void usage(char* name) {
    fprintf(stderr,"Usage : %s <filename> [options]\n", name);
    fprintf(stderr,"Where <filename> contains a corrupted AES key schedule that went through the binary erasure channel\n");
    fprintf(stderr,"For file formatting details, execute first ./bin/corruption <src_file> <probability> <channel_type> [options] or see ./samples/bin_erasure.txt\n");
    fprintf(stderr,"\nOptions :\n-verbose : v=true | v=false\n");
    exit(EXIT_FAILURE);
}

int parse_input(char*, int);

void correct_128();

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
        rows = 11;
        columns = 4;
    } else if (size > 4900) {
        key_length = 256;
        columns = 8;
        rows = 15;
    } else {
        key_length = 192;
        columns = 6;
        rows = 13;
    }

    nb_unknown = parse_input(raw, size);

    //Graphic parse confirmation
    if (VERBOSE) {
        printf("Parsed input : \n");
        int space = 0;
        for (int i = 0; i < rows; i++) {
            for(int j = 0; j < columns; j++)
                printf("%s ",grid[i][j]);
            printf("\n");
        }
        printf("\nType : AES-%d\nNumber of unknown bits : %d\n",key_length,nb_unknown);
    }

    //Backtrack
    if (key_length == 128)
        correct_128();

    return EXIT_SUCCESS;
}

//We transform the text file representing the output of a binary erasure channel 
//into a 3-dimensional 'grid' array where the first coordinate represents the turn, 
//the second the number of the word in a given turn and the third the value of the 32-bit word
int parse_input(char* raw, int size) {
    int x,y,z;
    int idx = 0;

    x = y = z = 0;
    for(int i = 0; i < size - 1; i++) {
        if (raw[i] == '\n') {
            x++;
            y = z = 0;
        } else if (raw[i] == ' ' && raw[i+1] == ' ') {
            grid[x][y][z] = '\0';
            y++;
            i++;
            z = 0;
        } else if (raw[i] != ' ') {
            grid[x][y][z] = raw[i];
            z++;
        }

        //ajout dans la liste des bits inconnus
        if (raw[i] == 'X') {
            unresolved[idx][0] = x;
            unresolved[idx][1] = y;
            unresolved[idx][2] = z - 1;
            idx++;
        }
    }

    return idx;
}

void correct_128() {
    for (int i = 0; i < nb_unknown; i++) {
        
    }
}