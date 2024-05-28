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

#endif