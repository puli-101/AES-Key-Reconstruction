#ifndef __CANDIDATE_HANDLER__H__
#define __CANDIDATE_HANDLER__H__

#include "util.h"
#include "resolution_handler.h"

#define BLOCK_SIZE 4    //size in bytes of each on of the four 4-byte-long blocks that define the first round key 
#define CANDIDATES 10   //maximum number of candidates considered at a time
#define NB_BLOCKS 4     //number of 32-bit blocks that define the first round key

typedef struct {
    uint8_t sub_key[BLOCK_SIZE];
    int block_nb;
    double score;
} candidate;

//list of all currently considered candidates
extern candidate cand_lst[NB_BLOCKS][CANDIDATES];

//prints a set of plausible candidates from one of the 4 32-bit blocks
void print_candidate_block(int block);

//given a new candidate (and its Z-score)
//determines if it is better suited than one
//of the currently stored candidates
void update_candidates(candidate* cand);

//prints all 4 blocks of plausible candidates
void print_candidates();

//sets an initial score to all candidates for all blocks
void init_candidates();

//copies candidate src to dest
void cpy_candidate(candidate* src, candidate* dest);

//just a bubble sort (we just use this function once so it's ok)
void sort_candidates(int block);

//Calculates hamming distance of candidate to the extracted solution
//and determines the candidates likelihood through the Z score
void calc_candidate_likelihood(candidate* cand,uint8_t grid[ROUNDS][NB_BYTES] , double expected_value, double std_deviation);

#endif