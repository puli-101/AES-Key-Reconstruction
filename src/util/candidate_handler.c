#include "candidate_handler.h"

candidate cand_lst[NB_BLOCKS][CANDIDATES];

void print_candidate_block(int block) {
    set_color(stdout,"yellow");
    printf("Possible candidates for block %d:\n",block);
    for (int j = 0; j < CANDIDATES; j++) {
        set_color(stdout,"cyan");
        printf("- Candidate %d : ", j);
        set_color(stdout,"default");
        for (int k = 0; k < 4; k++) 
            printf("%02x ", cand_lst[block][j].sub_key[k]);
        printf("\n%.3f %% likelihood\n\n", 100 * cand_lst[block][j].prob);
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