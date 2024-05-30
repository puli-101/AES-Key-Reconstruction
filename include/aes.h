#ifndef __AES_H__
#define __AES_H__

#include <stdio.h>
#include <stdint.h>

extern uint8_t sbox[256];

extern uint8_t rcon[255];

//substitution avec SBOX
uint32_t sub(uint32_t n);

//rotation d'un octet a gauche avec wrapping
uint32_t rot(uint32_t n);

//prints an AES key schedule of a specific number of rounds and words per round
void print_schedule(uint32_t schedule[15][8], int rounds, int nb_words);

#endif