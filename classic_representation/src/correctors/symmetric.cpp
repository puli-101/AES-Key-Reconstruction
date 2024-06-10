#include "aes.h"
#include "util.h"
#include <string>
#include <iostream>
using std::cout;
using std::endl;
using std::string;

void usage(char* name) {
    cout<<"Usage "<<name<<" <filename> <delta_0> <delta_1> [-v=false]"<<endl;
    cout<<" Where filename contains a noisy AES-128 key schedule"<<endl;
    cout<<" \tdelta_0 represents the probability of a bit set to 0 to flip to 1"<<endl;
    cout<<" \tdelta_1 represents the probability of a bit set to 1 to flip to 0"<<endl;
}

uint8_t grid[ROUNDS][NB_BYTES];

void print_schedule(uint8_t grid[ROUNDS][NB_BYTES]) {
    for (int i = 0; i < ROUNDS; i++) {
        for (int j = 0; j < NB_BYTES; j++) {
            if (j % 4 == 0) printf(" ");
            printf("%02x",grid[i][j]);
        }
        printf("\n");
    }
}

void correct();

int main(int argc, char** argv) {
    double delta0, delta1;
    char* fname;

    if (argc < 4) {
        usage(argv[0]);
    }


    //Option handling
    for (int i = 3; i < argc; i++) {
        if (!strcmp(argv[i],"-v=false"))
            VERBOSE = 0;
    }

    //Extraction of noisy key schedule
    char raw[MAX_SIZE];
    char *file = argv[1], *endPtr;
    int size = extract_text(file,raw);

    //Extraction of probability of decay of the channel
    delta0 = strtod( argv[2], &endPtr); 
    if (endPtr == argv[2]) {
        cout<<"Format error : cannot cast second argument into double"<<endl;
    } 
    delta1 = strtod( argv[3], &endPtr); 
    if (endPtr == argv[3]) {
        cout<<"Format error : cannot cast third argument into double"<<endl;
    } 

    //Parsing and parsing confirmation confirmation
    parse_input(raw, size, grid);
    cout<<"Parsed input :\n";
    print_schedule(grid);
    cout<<"\n(Presque) Bruteforcing kschedule...\n";
    
    correct();

    return EXIT_SUCCESS;


    return EXIT_SUCCESS;
}

void correct() {
    
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
 clé candidate que nous considérons, nous calculons le programme de 
 clé étendu et demandons si la probabilité que ce programme de clé étendu 
 se décompose en notre programme de clé récupéré est suffisamment élevée. 
 Si tel est le cas, nous affichons la clé correspondante à titre de supposition. 
 Lorsque l’un des δ0 ou δ1 est très petit, cet algorithme produira 
 presque certainement une estimation unique de la clé.
*/