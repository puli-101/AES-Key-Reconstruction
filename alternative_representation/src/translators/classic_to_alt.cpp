#include "aes.h"
#include "util.h"
#include <string>
#include <iostream>
using std::cout;
using std::endl;
using std::string;

void usage(char* name) {
    cout<<"Usage "<<name<<" <filename>"<<endl;
    cout<<"- Where filename contains an AES-128 key schedule"<<endl;
    exit(EXIT_FAILURE);
}

void set_up(uint8_t grid[ROUNDS][NB_BYTES], uint8_t alternative[ROUNDS][NB_BYTES]);

uint8_t grid[ROUNDS][NB_BYTES];
uint8_t alternative[ROUNDS][NB_BYTES];

void print_schedule(uint8_t grid[ROUNDS][NB_BYTES]) {
    for (int i = 0; i < ROUNDS; i++) {
        for (int j = 0; j < NB_BYTES; j++) {
            if (j % 4 == 0) printf(" ");
            printf("%02x",grid[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    double delta0, delta1;
    char* fname;

    if (argc < 2) {
        usage(argv[0]);
    }

    //Option handling
    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i],"-v=false"))
            VERBOSE = 0;
    }

    //Extraction of key schedule
    char raw[MAX_SIZE];
    char *file = argv[1], *endPtr;
    int size = extract_text(file,raw);

    //Parsing and parsing confirmation confirmation
    parse_input(raw, size, grid);
    if (VERBOSE) {
        cout<<"Parsed input :\n";
        print_schedule(grid);
        cout<<"\nTranslation of kschedule...\n";
    }
    set_up(grid, alternative);

    return EXIT_SUCCESS;
}

void set_up(uint8_t grid[ROUNDS][NB_BYTES], uint8_t alternative[ROUNDS][NB_BYTES]) {

    for (int i = 0; i < ROUNDS; i++) {
        alternative[i][0] = grid[i][15];
        alternative[i][4] = grid[i][14];
        alternative[i][8] = grid[i][13];
        alternative[i][12] = grid[i][12];

        alternative[i][1] = grid[i][14] ^ grid[i][10] ^ grid[i][6] ^ grid[i][2];
        alternative[i][5] = grid[i][13] ^ grid[i][9] ^ grid[i][5] ^ grid[i][1];
        alternative[i][9] = grid[i][12] ^ grid[i][8] ^ grid[i][4] ^ grid[i][0];
        alternative[i][13] = grid[i][15] ^ grid[i][11] ^ grid[i][7] ^ grid[i][3];

        alternative[i][2] = grid[i][13] ^ grid[i][5];
        alternative[i][6] = grid[i][12] ^ grid[i][4];
        alternative[i][10] = grid[i][15] ^ grid[i][7];
        alternative[i][14] = grid[i][14] ^ grid[i][6];

        alternative[i][3] = grid[i][12] ^ grid[i][8];
        alternative[i][7] = grid[i][15] ^ grid[i][11];
        alternative[i][11] = grid[i][14] ^ grid[i][10];
        alternative[i][15] = grid[i][13] ^ grid[i][9];

        for (int j = 0; j < NB_BYTES; j++) {
            if (j%4 == 0) printf(" ");
            printf("%02x",alternative[i][j]);
        }
        printf("\n");
    }
}

/*
 Pour chaque ensemble possible de 4 octets de clé, 
 nous générons les trois octets pertinents de la sous clé  
 suivante et calculons la probabilité, compte tenu des 
 estimations de δ0 et δ1, que ces sept octets aient pu se 
 dégrader en octets correspondants des sous clés récupérées. 
 Nous procédons à deviner les clés candidates, où un candidat 
 contient une valeur pour chaque tranche d'octets. 

 Nous considérons les candidats par ordre de probabilité 
 totale décroissante tel que calculé ci-dessus. Pour chaque
 clé candidate que nous considérons, nous calculons le keyschedule
 étendu et demandons si la probabilité que ce programme de clé étendu 
 se décompose en notre programme de clé récupéré est suffisamment élevée. 
 Si tel est le cas, nous affichons la clé correspondante à titre de supposition. 
 Lorsque l’un des δ0 ou δ1 est très petit, cet algorithme produira 
 presque certainement une estimation unique de la clé.
*/