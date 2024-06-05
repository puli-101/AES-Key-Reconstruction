#ifndef __CANDIDATE_HANDLER__H__
#define __CANDIDATE_HANDLER__H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "util.h"
#define BLOCK_SIZE 4
#define CANDIDATES 10
#define NB_BLOCKS 4

typedef struct {
    uint8_t sub_key[BLOCK_SIZE];
    int block_nb;
    double score;
} candidate;

extern candidate cand_lst[NB_BLOCKS][CANDIDATES];
void print_candidate_block(int block);
void update_candidates(candidate*);
void print_candidates();
void init_candidates();
#endif