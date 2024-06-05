#include "candidate_handler.h"

candidate cand_lst[NB_BLOCKS][CANDIDATES];

//copies candidate src to dest
void cpy_candidate(candidate* src, candidate* dest) {
    dest->block_nb = src->block_nb;
    dest->score = src->score;
    for (int i = 0; i < BLOCK_SIZE; i++)
        dest->sub_key[i] = src->sub_key[i];
} 

//just a bubble sort (we dont use this function often so it's ok)
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

void print_candidates() {
    for (int i = 0; i < NB_BLOCKS; i++) {
        print_candidate_block(i);
    }
}

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

void init_candidates() {
    for (int i = 0; i < NB_BLOCKS; i++) {
        for (int j = 0; j < CANDIDATES; j++) {
            cand_lst[i][j].score = 100;
        }
    }
}