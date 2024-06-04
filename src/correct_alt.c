#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "util.h"
#include <limits.h>
#define MAX_SIZE 8192
#define CANDIDATES 10
#define NB_BLOCKS 4
#define BLOCK_SIZE 4

//Naive key reconstruction algorithm for AES-128 keys that went through the binary noisy channels
//sample execution : ./bin/correct_alt alternative/sched1_bsc_0625 0.0625 0.0625 -v=false

uint8_t grid[ROUNDS][NB_BYTES];       //representation ascii d'un key schedule

typedef struct {
    uint8_t sub_key[BLOCK_SIZE];
    int block_nb;
    double prob;
} candidate;

candidate cand_lst[NB_BLOCKS][CANDIDATES];

void update_candidates(candidate*);
void print_candidates();
void correct();
void calc_prob(candidate*);
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

    for (int i = 2; i < argc; i++) {
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

    check(cand);
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
            if (VERBOSE && (j % 214748364 == 0)) {
                print_progress(prcntg);
                prcntg += 0.05;
            }

            for (int k = 0; k < BLOCK_SIZE; k++)
                cand->sub_key[k] = get_byte_from_word(j,k);
            
            calc_prob(cand);
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


void calc_prob(candidate* cand) {
    
}

