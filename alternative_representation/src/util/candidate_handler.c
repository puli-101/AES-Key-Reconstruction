#include "candidate_handler.h"

candidate cand_lst[NB_BLOCKS][CANDIDATES];

//copies candidate src to dest
void cpy_candidate(candidate* src, candidate* dest) {
    dest->block_nb = src->block_nb;
    dest->score = src->score;
    for (int i = 0; i < BLOCK_SIZE; i++)
        dest->sub_key[i] = src->sub_key[i];
} 

//just a bubble sort (we just use this function once so it's ok)
void sort_candidates(int block) {
    candidate temp; 
    for (int i = 0; i < CANDIDATES; i++) {
        for (int j = 0; j < CANDIDATES - 1; j++) {
            if (cand_lst[block][j].score > cand_lst[block][j+1].score) {
                cpy_candidate(&cand_lst[block][j+1], &temp);
                cpy_candidate(&cand_lst[block][j], &cand_lst[block][j+1]);
                cpy_candidate(&temp, &cand_lst[block][j]);
            }
        }
    }
}

//prints a set of plausible candidates from one of the 4 32-bit blocks
void print_candidate_block(int block) {
    int limit = SHORTENED ? 3 : CANDIDATES;
    sort_candidates(block);
    set_color(stdout,"yellow");
    printf("Possible candidates for block %d:\n",block);
    for (int j = 0; j < limit; j++) {
        set_color(stdout,"cyan");
        printf("- Candidate %d : ", j);
        set_color(stdout,"default");
        for (int k = 0; k < 4; k++) 
            printf("%02x ", cand_lst[block][j].sub_key[k]);
        printf("\n%.3f score\n\n", cand_lst[block][j].score);
    }
}

//prints all 4 blocks of plausible candidates
void print_candidates() {
    for (int i = 0; i < NB_BLOCKS; i++) {
        print_candidate_block(i);
    }
}

//given a new candidate (and its Z-score)
//determines if it is better suited than one
//of the currently stored candidates
void update_candidates(candidate* cand) {
    int block = cand->block_nb;
    double max_score = 0;
    candidate* to_replace = NULL;
    //search min
    for (int i = 0; i < CANDIDATES; i++) {
        if (cand_lst[block][i].score > max_score) {
            max_score = cand_lst[block][i].score;
            to_replace = &cand_lst[block][i];
        }
    }
    //replacement
    if (to_replace != NULL && max_score > cand->score) {
        to_replace->score = cand->score;
        for (int i = 0; i < BLOCK_SIZE; i++)
            to_replace->sub_key[i] = cand->sub_key[i];
    }
}

//sets an initial score to all candidates for all blocks
void init_candidates() {
    for (int i = 0; i < NB_BLOCKS; i++) {
        for (int j = 0; j < CANDIDATES; j++) {
            cand_lst[i][j].score = 100;
        }
    }
}

//Calculates hamming distance of candidate to the extracted solution
//and determines the candidates likelihood through the Z score
void calc_candidate_likelihood(candidate* cand,uint8_t grid[ROUNDS][NB_BYTES] , double expected_value, double std_deviation) {
    uint8_t subschedule[ROUNDS][BLOCK_SIZE];
    int diff;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        subschedule[0][i] = cand->sub_key[i];
    }
    calc_subschedule(subschedule, cand->block_nb);
    diff = calc_diff(subschedule, grid, cand->block_nb);
    cand->score = z_score(diff,expected_value, std_deviation);
}