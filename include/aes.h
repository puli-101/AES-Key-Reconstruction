#ifndef __AES_H__
#define __AES_H__

#include <stdio.h>
#include <stdint.h>
#include "util.h"
#define NB_BYTES 16
#define ROUNDS 11
#define NB_WORDS 4

extern uint8_t sbox[256];

extern uint8_t rcon[255];

//substitution avec SBOX
uint32_t sub(uint32_t n);

//rotation d'un octet a gauche avec wrapping
uint32_t rot(uint32_t n);

//prints an AES key schedule of a specific number of rounds and words per round
void print_schedule(uint32_t schedule[15][NB_WORDS], int rounds);

//prints the alternative version of the representation of the key schedule
void print_new_schedule(uint8_t s[ROUNDS][NB_BYTES]);

#endif