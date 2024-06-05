#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"
#include "util.h"
#include "candidate_handler.h"
#define MAX_SIZE 8192
#define SUB_SCHED_SIZE 352 //size of a subschedule in bits
//Naive key reconstruction algorithm for AES-128 keys that went through the binary noisy channels
//sample execution : ./bin/correct_alt alternative/sched1_bsc_0625 0.0625 0.0625 -v=false

uint8_t grid[ROUNDS][NB_BYTES];       //representation ascii d'un key schedule
double proba0, proba1;
double std_deviation;
double expected_value;

void calc_subschedule(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int index);
void calc_candidate_likelihood(candidate*);

void correct();
int calc_diff(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int offset);

double z_score(int diff) {
    return abs_double((((double)diff) - expected_value) / std_deviation);
}

void usage(char* name) {
    print_header(name,"<filename> <delta0> <delta1> [options]");
    fprintf(stderr,"Where\n - \033[0;36m<filename>\033[0m contains a corrupted AES key schedule that went through a binary noisy channel\n");
    fprintf(stderr,"- \033[0;36m<delta0>\033[0m represents the probability of a bit set to 0 to becoma a 1\n");
    fprintf(stderr,"- \033[0;36m<delta1>\033[0m represents the probability of a bit set to 1 to decay to 0\n");
    fprintf(stderr,"For file formatting details, execute first ./bin/noise <src_file> <probability> <channel_type> [options] ");
    fprintf(stderr, "or see ./samples/sched1_bsc.txt\n");
    print_color(stderr, "Options :","yellow",'\n');
    fprintf(stderr,"- \033[0;36mverbose\033[0m : -v=true | -v=false (true by default)\n");
    fprintf(stderr,"- \033[0;36mshortened list of candidate key schedules\033[0m : -s=true | -s=false (false by default)\n");
    fprintf(stderr,"- \033[0;36mrandomized search\033[0m : -r=true | -r=false (false by default)\n");
    fprintf(stderr,"- \033[0;36mlimit maximum number of iterations\033[0m : -l=<n> where <n> is a natural number representing the maximum number of itereations\n");
    exit(EXIT_FAILURE);
}

void parse_input(char*, int);

void correct();

void test();

int main(int argc, char** argv) {
    if (argc < 4) {
        usage(argv[0]);
    }

    for (int i = 4; i < argc; i++) {
        if (!strcmp(argv[i],"-v=false"))
            VERBOSE = 0;
        if (!strcmp(argv[i],"-s=true"))
            SHORTENED = 1;
    }

    char raw[MAX_SIZE];
    char *file = argv[1], *endPtr;
    int size = extract_text(file,raw);

    //Probability of decay from 0 to 1
    proba0 = strtod( argv[2], &endPtr); 
    if (endPtr == argv[2]) {
        print_color(stderr,"Format error : cannot cast third argument into double","red",'\n');
        usage(argv[0]);
    } 
    std_deviation = sqrt(SUB_SCHED_SIZE * proba0 * (1-proba0));
    expected_value = SUB_SCHED_SIZE * proba0;
    
    //Probability of decay from 1 to 0
    proba1 = strtod( argv[3], &endPtr); 
    if (endPtr == argv[3]) {
        print_color(stderr,"Format error : cannot cast fourth argument into double","red",'\n');
        usage(argv[0]);
    }  

    parse_input(raw, size);
    
    //Surse confirmation
    if (VERBOSE) {
        print_color(stdout,"Parsed input :","yellow",'\n');
        print_new_schedule(grid);
        print_color(stdout,"\nBruteforcing kschedule...","yellow",'\n');
    }
    
    //test();
    init_candidates();
    correct();

    return EXIT_SUCCESS;
}

void parse_input(char* raw, int size) {
    char hex[3], *end;
    int x = 0, y = 0;

    for (int i = 0; i < size - 1; i++) {
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
        i++;
    }
}

void correct() {
    candidate* cand = (candidate*)malloc(sizeof(candidate));
    double prcntg;
    uint32_t nb_iter = UINT32_MAX;
    int threshold = ((double)nb_iter * 0.0125);
    //check(cand);
    for (int i = 0; i < NB_BLOCKS; i++) {
        if (VERBOSE) {
            set_color(stdout,"yellow");
            printf("Beginning bruteforcing of %dth block...\n",i);
            set_color(stdout,"default");
            prcntg = 0;
        }
        cand->block_nb = i;
        //bruteforcing over all possible initial vectors
        for (uint32_t j = 0; j < nb_iter; j++) { 
            if (VERBOSE && (j % threshold == 0)) {
                print_progress(prcntg);
                prcntg += 0.0125;
            }

            for (int k = 0; k < BLOCK_SIZE; k++)
                cand->sub_key[k] = get_byte_from_word(j,k);
            
            calc_candidate_likelihood(cand);
            update_candidates(cand);
            //if (cand->score < 1.5)
            //    break;
        }
        

        print_progress(1);
        printf("\n");
        print_candidate_block(i);
    }
    free(cand);
}

void calc_subschedule(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int index) {
    for (int i = 1; i < ROUNDS; i++) {
        subschedule[i][0] = subschedule[i-1][1] ^ sbox[subschedule[i-1][0]];
        subschedule[i][1] = subschedule[i-1][2];
        subschedule[i][2] = subschedule[i-1][3];
        subschedule[i][3] = subschedule[i-1][0];
        if (index % 4 == 2) {
            subschedule[i][0] ^= rcon[i];
        }
        index = (index + 1) % 4;
    }
    
}

//Calculates the hamming distance between a subschedule 
//i.e. the slice of an alternative key schedule generated by 4 contiguous bytes, and
//the corresponding subschedule from the originally extracted key schedule 
//index : {0,1,2,3} refers to the word number in the first round of the alternative schedule
int calc_diff(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int offset) {
    int diff = 0, delta = 0;
    uint8_t byte_diff;

    for (int i = 0; i < ROUNDS; i++) {
        delta = 0;
        for (int j = 0; j < BLOCK_SIZE; j++) {
            byte_diff = subschedule[i][j] ^ grid[i][(j + 4*(offset+i))%NB_BYTES];
            for (int k = 0; k < 8; k++) {
                if ( (1 << k) & byte_diff)
                    delta++;
            }
        }
        diff += delta;
    }
    return diff;
}

void calc_candidate_likelihood(candidate* cand) {
    uint8_t subschedule[ROUNDS][BLOCK_SIZE];
    int diff;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        subschedule[0][i] = cand->sub_key[i];
    }
    calc_subschedule(subschedule, cand->block_nb);
    diff = calc_diff(subschedule, cand->block_nb);
    cand->score = z_score(diff);
}

void test() {
    uint8_t subschedule[ROUNDS][BLOCK_SIZE];
    
    subschedule[0][0] = 0x00;
    subschedule[0][1] = 0x00;
    subschedule[0][2] = 0x00;
    subschedule[0][3] = 0x88;
    calc_subschedule(subschedule,0);
    int diff = calc_diff(subschedule,0);
    printf("Diff : %d \n",diff);
    printf("Likelihood : %.3f %%\n", z_score(diff));
}