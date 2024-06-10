#ifndef _RESOLUTION_HANDLER_H_
#define _RESOLUTION_HANDLER_H_

#include <stdint.h>
#include "aes.h"
#include "util.h"

#define SCALE 1
#define BLOCK_SIZE 4
#define NB_BLOCKS 4
#define MAX_SIZE 8192
#define SUB_SCHED_SIZE (int)(BLOCK_SIZE * ROUNDS * 8 * SCALE) //size of a subschedule in bits
//Naive key reconstruction algorithm for AES-128 keys that went through the binary noisy channels
//sample execution : ./bin/correct_alt alternative/sched1_bsc_0625 0.0625 0.0625 -v=false
int calc_diff(uint8_t subschedule[ROUNDS][BLOCK_SIZE], uint8_t grid[ROUNDS][NB_BYTES], int offset);

void calc_subschedule(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int index);

static inline double z_score(int diff, double expected_value, double std_deviation) {
    return abs_double((((double)diff) - expected_value) / std_deviation);
}

#endif