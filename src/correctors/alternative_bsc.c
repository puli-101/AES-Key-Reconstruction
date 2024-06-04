#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "util.h"
#include <limits.h>
#include <math.h>
#define MAX_SIZE 8192
#define CANDIDATES 10
#define NB_BLOCKS 4
#define BLOCK_SIZE 4
#define SUB_SCHED_SIZE 352 //size of a subschedule in bits
//Naive key reconstruction algorithm for AES-128 keys that went through the binary noisy channels
//sample execution : ./bin/correct_alt alternative/sched1_bsc_0625 0.0625 0.0625 -v=false

uint8_t grid[ROUNDS][NB_BYTES];       //representation ascii d'un key schedule
double proba0;

typedef struct {
    uint8_t sub_key[BLOCK_SIZE];
    int block_nb;
    double prob;
} candidate;

candidate cand_lst[NB_BLOCKS][CANDIDATES];
void calc_subschedule(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int index);

void update_candidates(candidate*);
void print_candidates();
void correct();
void calc_candidate_likelihood(candidate*);
void print_candidate_block(int block);

void usage(char* name) {
    print_header(name,"<filename> <delta0> <delta1> [options]");
    fprintf(stderr,"Where\n - \033[0;36m<filename>\033[0m contains a corrupted AES key schedule that went through a binary noisy channel\n");
    fprintf(stderr,"- \033[0;36m<delta0>\033[0m represents the probability of a bit set to 0 to becoma a 1\n");
    fprintf(stderr,"- \033[0;36m<delta1>\033[0m represents the probability of a bit set to 1 to decay to 0\n");
    fprintf(stderr,"For file formatting details, execute first ./bin/corruption <src_file> <probability> <channel_type> [options] or see ./samples/sched1_bsc.txt\n");
    print_color(stderr, "Options :","yellow",'\n');
    fprintf(stderr,"- \033[0;36mverbose\033[0m : v=true | v=false\n");
    exit(EXIT_FAILURE);
}

void parse_input(char*, int);

void correct();

int main(int argc, char** argv) {
    if (argc < 4) {
        usage(argv[0]);
    }

    for (int i = 4; i < argc; i++) {
        if (!strcmp(argv[i],"-v=false"))
            VERBOSE = 0;
    }

    char raw[MAX_SIZE];
    char *file = argv[1];
    int size = extract_text(file,raw);

    parse_input(raw, size);
    
    //Graphic parse confirmation
    if (VERBOSE) {
        print_color(stdout,"Parsed input :","yellow",'\n');
        print_new_schedule(grid);
        print_color(stdout,"\nBruteforcing kschedule...","yellow",'\n');
    }
    /*uint8_t subschedule[ROUNDS][BLOCK_SIZE];
    printf("0 :");
    subschedule[0][0] = 0x00;
    subschedule[0][1] = 0x00;
    subschedule[0][2] = 0x00;
    subschedule[0][3] = 0x88;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        subschedule[0][i] = grid[0][i];
        printf("%02x ",subschedule[0][i]);
    }
    printf("\n");
    calc_subschedule(subschedule,0);*/
    correct();
    print_candidates();

    return EXIT_SUCCESS;
}

void parse_input(char* raw, int size) {
    char hex[3], *end;
    int x = 0, y = 0;

    for (int i = 0; i < size - 1; i+=2) {
        if (raw[i] == '\n' || raw[i] == ' ') continue;
        if (raw[i] == '\0' || raw[i] == EOF) break;
        hex[0] = raw[i];
        hex[1] = raw[i+1];
        hex[2] = '\0';
        grid[x][y] = strtol(hex, &end, 16); 
            
        if (end == hex) {
            print_color(stdout, "Error while parsing file","red",'\n');
            usage("./executable_name");
        }
        y++;
        if (y >= NB_BYTES) {
            y = 0;
            x++;

            if (x > ROUNDS) {
                print_color(stdout, "Error, input file too large","red",'\n');
                usage("./executable_name");
            }
        } 
    }
}

void correct() {
    candidate* cand = (candidate*)malloc(sizeof(candidate));
    double prcntg;

    //check(cand);
    for (int i = 0; i < NB_BLOCKS; i++) {
        if (VERBOSE) {
            set_color(stdout,"yellow");
            printf("Beginning bruteforcing of %dth block...\n",(i+1));
            set_color(stdout,"default");
            prcntg = 0;
        }
        cand->block_nb = i;
        //bruteforcing over all possible initial vectors
        for (uint32_t j = 0; j < UINT_MAX; j++) {
            if (VERBOSE && (j % 26843546 == 0)) {
                print_progress(prcntg);
                prcntg += 0.00625;
            }

            for (int k = 0; k < BLOCK_SIZE; k++)
                cand->sub_key[k] = get_byte_from_word(j,k);
            
            calc_candidate_likelihood(cand);
            update_candidates(cand);
        }
        

        print_progress(1);
        print_color(stdout, "\nPossible candidates :","yellow",'\n');
        print_candidate_block(i);
    }
    free(cand);
}

void print_candidate_block(int block) {
    set_color(stdout,"yellow");
    printf("Possible candidates for block %d:\n",block);
    for (int j = 0; j < CANDIDATES; j++) {
        set_color(stdout,"cyan");
        printf("- Candidate %d : ", j);
        set_color(stdout,"default");
        for (int k = 0; k < 4; k++) 
            printf("%02x ", cand_lst[block][j].sub_key[k]);
        printf("\n%.3f %% likelihood\n\n", cand_lst[block][j].prob);
    }
}

void print_candidates() {
    for (int i = 0; i < NB_BLOCKS; i++) {
        print_candidate_block(i);
    }
}

void update_candidates(candidate* cand) {
    int block = cand->block_nb;
    double min_prob = 1;
    candidate* to_replace = NULL;
    //search min
    for (int i = 0; i < CANDIDATES; i++) {
        if (cand_lst[block][i].prob < min_prob) {
            min_prob = cand_lst[block][i].prob;
            to_replace = &cand_lst[block][i];
        }
    }
    //replacement
    if (to_replace != NULL && min_prob < cand->prob) {
        to_replace->prob = cand->prob;
        for (int i = 0; i < BLOCK_SIZE; i++)
            to_replace->sub_key[i] = cand->sub_key[i];
    }
}


void calc_subschedule(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int index) {
    for (int i = 1; i < ROUNDS; i++) {
        subschedule[i][0] = subschedule[i-1][1] ^ sbox[subschedule[i-1][0]];
        subschedule[i][1] = subschedule[i-1][2];
        subschedule[i][2] = subschedule[i-1][3];
        subschedule[i][3] = subschedule[i-1][0];
        if (index % 4 == 2)
            subschedule[i][0] ^= rcon[i];
        index = (index + 1) % 4;
        /*printf("%d :",i);
        for (int j = 0; j < BLOCK_SIZE; j++)
            printf("%02x ", subschedule[i][j]);
        printf("\n");*/
    }
    
}

int choose(int n, int k) {
    return tgamma(n + 1)/(tgamma(n + 1) * (tgamma(n - k + 1)));
}

int calc_diff(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int index) {
    int diff = 0;
    uint8_t byte_diff;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        byte_diff = subschedule[1][i] ^ grid[1][(i+4)%NB_BYTES];
        for (int j = 0; j < 8; j++) {
            if ( (1 << j) & byte_diff)
                diff++;
        }
    }
    return diff;
}

void calc_candidate_likelihood(candidate* cand) {
    uint8_t subschedule[ROUNDS][BLOCK_SIZE];
    int diff;
    for (int i = 0; i < BLOCK_SIZE; i++)
        subschedule[0][i] = cand->sub_key[i];
    calc_subschedule(subschedule, cand->block_nb);
    diff = calc_diff(subschedule, cand->block_nb);
    //change 8 for something else
    cand->prob = choose(32,diff) * pow(proba0, diff) * pow((1.0-proba0), 32 - diff);
}

//Salut, 
//Je sais pas si je me trompe mais pour calculer les probas de chaque clé candidat je considère une variable aléatoire X de loi binomiale de paramètres n : la taille en bits du segment du key schedule engendré par la sous clé candidate et p la probabilité qu'un bit soit inversé
//et puis la proba de la sous clé candidat est la proba de { X = DistanceHamming(sous key schedule engendré et sous key schedule dans la RAM) },

// est-ce que c'est le cas ou faut il faire une modelisation des p
//Alternativement ce que je pensait de faire etait d'enchainer et additioner plusieurs probas conditionels du style "proba qu'un certain bit soit egal a 'x' sachant que le bit du round dernier etait egal a 'y'" mais je ne sais pas si la premiere option suffit
