#include "candidate_handler.h"
#include "resolution_handler.h"
#define MAX_SIZE 8192

//Naive key reconstruction algorithm for AES-128 keys that went through the binary symmetric channel
//sample execution : ./bin/correct_alt alternative/sched1_bsc_0625 0.0625 0.0625 -v=false

uint8_t grid[ROUNDS][NB_BYTES];     //Representation of extracted key schedule as a grid
double proba;                       //probability of decay of the bsc
double std_deviation;               //Standard deviation of the random variable X
double expected_value;              //Expected value of random variable X

//X : random binomial variable of parameters p = proba and N = size of a key schedule slice in bits

void usage(char* name) {
    print_header(name,"<filename> <delta0> [options]");
    fprintf(stderr,"Where\n - \033[0;36m<filename>\033[0m contains a corrupted AES key schedule that went through a binary noisy channel\n");
    fprintf(stderr,"- \033[0;36m<delta0>\033[0m represents the probability of a bit set to 0 to becoma a 1\n");
    fprintf(stderr,"For file formatting details, execute first ./bin/noise <src_file> <probability> <channel_type> [options] ");
    fprintf(stderr, "or see ./samples/sched1_bsc.txt\n");
    print_color(stderr, "Options :","yellow",'\n');
    fprintf(stderr,"- \033[0;36mverbose\033[0m : -v=true | -v=false (true by default)\n");
    fprintf(stderr,"- \033[0;36mshortened list of candidate key schedules\033[0m : -s=true | -s=false (false by default)\n");
    fprintf(stderr,"- \033[0;36mrandomized search\033[0m : -r=true | -r=false (false by default)\n");
    fprintf(stderr,"- \033[0;36mlimit maximum number of iterations\033[0m : -l=<n> where <n> is a natural number representing the maximum number of itereations\n");
    exit(EXIT_FAILURE);
}

void correct();

int main(int argc, char** argv) {
    if (argc < 3) {
        usage(argv[0]);
    }

    //Option handling
    for (int i = 3; i < argc; i++) {
        if (!strcmp(argv[i],"-v=false"))
            VERBOSE = 0;
        if (!strcmp(argv[i],"-s=true"))
            SHORTENED = 1;
    }

    //Extraction of noisy key schedule
    char raw[MAX_SIZE];
    char *file = argv[1], *endPtr;
    int size = extract_text(file,raw);

    //Setting up decay probability and X
    proba = strtod( argv[2], &endPtr); 
    if (endPtr == argv[2]) {
        print_color(stderr,"Format error : cannot cast third argument into double","red",'\n');
        usage(argv[0]);
    } 
    std_deviation = sqrt(SUB_SCHED_SIZE * proba * (1-proba));
    expected_value = SUB_SCHED_SIZE * proba;

    //Parsing and parsing confirmation confirmation
    parse_input(raw, size, grid);
    if (VERBOSE) {
        print_color(stdout,"Parsed input :","yellow",'\n');
        print_new_schedule(grid);
        print_color(stdout,"\nBruteforcing kschedule...","yellow",'\n');
    }
    
    init_candidates();
    correct();

    return EXIT_SUCCESS;
}

//Begins error correction
void correct() {
    candidate* cand = (candidate*)malloc(sizeof(candidate));    //candidate to be analyzed
    double prcntg;                                          //progress percentage
    uint32_t nb_iter = UINT32_MAX;                          //maximum number of iterations
    int threshold = ((double)nb_iter * 0.0125);             //threshold for percentage refresh
    
    //Bruteforcing for each of the 4 blocks of 32 bits
    for (int i = 0; i < NB_BLOCKS; i++) {
        if (VERBOSE) {
            set_color(stdout,"yellow");
            printf("Beginning bruteforcing of %dth block...\n",i);
            set_color(stdout,"default");
            prcntg = 0;
        }
        cand->block_nb = i;
        //Bruteforcing over all possible initial vectors
        for (uint32_t j = 0; j < nb_iter; j++) { 
            if (VERBOSE && (j % threshold == 0)) {
                print_progress(prcntg);
                prcntg += 0.0125;
            }

            for (int k = 0; k < BLOCK_SIZE; k++)
                cand->sub_key[k] = get_byte_from_word(j,k);
            
            calc_candidate_likelihood(cand,grid,expected_value,std_deviation);
            update_candidates(cand);
            //if the Z-score is less than 1.5 then we have a very good guess
            //of the possible value of the initial block (if p is small enough)
            if (cand->score < 1.5)
                break;
        }
        

        print_progress(1);
        printf("\n");
        print_candidate_block(i);
    }
    free(cand);
}
