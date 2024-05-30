#include <stdio.h>
#include <stdlib.h>
#include "aes.h"
#include "util.h"
#define MAX_SIZE 8192

//sample execution : ./bin/correct_bec samples/aes-128-bin_erasure.txt -v=false

char grid[15][8][33];       //representation ascii d'un key schedule
int key_length;             //taille de la clef aes (128,192,256)
list* unresolved;           //liste de cordonnees des bits inconnus
int nb_unknown;             //nombre de bits inconnus
int rows, columns;

void usage(char* name) {
    fprintf(stderr,"Usage : %s <filename> [options]\n", name);
    fprintf(stderr,"Where <filename> contains a corrupted AES key schedule that went through the binary erasure channel\n");
    fprintf(stderr,"For file formatting details, execute first ./bin/corruption <src_file> <probability> <channel_type> [options] or see ./samples/bin_erasure.txt\n");
    fprintf(stderr,"\nOptions :\n-verbose : v=true | v=false\n");
    exit(EXIT_FAILURE);
}

void parse_input(char*, int);

void correct_128();

void print_grid();

char ascii_xor(char, char);

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

    parse_input(raw, size);

    //Graphic parse confirmation
    if (VERBOSE) {
        printf("Parsed input : \n");
        print_grid();
        printf("\nType : AES-%d\nNumber of unknown bits : %d\n",key_length,nb_unknown);
    }

    //Backtrack
    if (key_length == 128)
        correct_128();

    free_list(&unresolved);
    return EXIT_SUCCESS;
}

//We transform the text file representing the output of a binary erasure channel (given an aes key schedule as an input)
//into a 3-dimensional 'grid' array where the first coordinate represents the round, 
//the second the position of a word in a given round and the third the value of the 32-bit word
void parse_input(char* raw, int size) {
    int x,y,z;

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
            int coord[] = {x,y,z-1};
            insert(&unresolved,coord,3);
            nb_unknown++;
        }
    }
}

int core() {
    int solved = 0;
    int x, y, z;
    char top,left, bottom, bleft, right, tright;
    list* next;
    
    
    //preliminary resolution of internal bits TOP-LEFT APPROACH
    //(bits that aren't in words in the first column or first row)
    for (list* iter = unresolved; iter;) {
        next = iter->next;
        x = iter->data[0];
        y = iter->data[1];
        z = iter->data[2]; 
        if (!(x == 0 || y == 0 || grid[x][y][z] != 'X') 
                && !(grid[x-1][y][z] == 'X' || grid[x][y-1][z] == 'X')) {
            top = grid[x-1][y][z];
            left = grid[x][y-1][z];
            if (top == 'X' || left == 'X') continue;
            grid[x][y][z] = ascii_xor(left,top);
            solved++;
            delete_elt(&unresolved, iter);
            printf("(%d,%d,%d)\n",x,y,z);
        }
        iter = next;
    }  
    //preliminary resolution of internal bits BOTTOM/BOTTOM-LEFT APPROACH
    for (list* iter = unresolved; iter;)  {
        x = iter->data[0];
        y = iter->data[1];
        z = iter->data[2];  
        next = iter->next;
        if (!(x == (rows - 1) || y == 0 || grid[x][y][z] != 'X') &&
                !(grid[x+1][y][z] == 'X' || grid[x+1][y-1][z] == 'X')) { 
            
            bottom = grid[x+1][y][z];
            bleft = grid[x+1][y-1][z];
            //xor a la main
            grid[x][y][z] = ascii_xor(bottom, bleft);
            solved++;
            delete_elt(&unresolved, iter);
        }
        iter = next;
    }
    //preliminary resolution of internal bits RIGHT/TOP-RIGHT APPROACH
    for (list* iter = unresolved; iter;) {
        x = iter->data[0];
        y = iter->data[1];
        z = iter->data[2]; 
        next = iter->next;  
        if (!(x == 0 || y == (columns - 1) || grid[x][y][z] != 'X') 
                && !(grid[x][y+1][z] == 'X' || grid[x-1][y+1][z] == 'X')) {
            right = grid[x][y+1][z];
            tright = grid[x-1][y+1][z];
            //xor a la main
            grid[x][y][z] = ascii_xor(right, tright);
            solved++;
            delete_elt(&unresolved, iter);
        }
        iter = next;
    }
    return solved;
}

void correct_128() {
    int solved = 0, delta;
    if (VERBOSE)
        printf("Preliminary resolution...\n");

    do {
        delta = core();
        if (VERBOSE)
            printf("Resolved %d more bits...\n",delta);
        solved += delta;
    } while(delta > 0);

    if (VERBOSE) {
        printf("Resolved %.3f %% unknown bits\n", ((float)100 * solved/(float)nb_unknown));
        print_grid();
        printf("\n");
    }
}

void print_grid() {
    for (int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++)
            printf("%s ",grid[i][j]);
        printf("\n");
    }
}

char ascii_xor(char a, char b) {
    if ((a == '0' || a == '1') && (b == '0' || b == '1'))
        return ((a == '1' && b == '1') || (a == '0' && b == '0')) ? '0' : '1';
    else
        fprintf(stderr,"Error while x-oring a : %c , b : %c \n",a,b);
    exit(EXIT_FAILURE);
}