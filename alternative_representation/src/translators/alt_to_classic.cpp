#include "aes.h"
#include "util.h"
#include <string>
#include <iostream>
using std::cout;
using std::endl;
using std::string;

void usage(char* name) {
    cout<<"Usage "<<name<<" <filename> [-v=false]"<<endl;
    cout<<"- Where filename contains an AES-128 key schedule"<<endl;
    exit(EXIT_FAILURE);
}

//Given 'skey' a string representing a k-bit long number in hexadecimal
//it casts every 8 consecutive characters into a 32-bit word
//and stores it in 'key'
void parse_input_key(char* skey, uint8_t key[NB_BYTES]) {
    char word[3];
    word[2] = '\0';
    for (int i = 0; i < NB_BYTES * 2; i+=2) {
        for (int j = 0; j < 2; j++)
            word[j] = skey[i + j];
        key[i/2] = (int)strtol(word, NULL, 16);
    }

    if (VERBOSE) {
        cout<<"Parsed key"<<endl;
        for (int i = 0; VERBOSE && i < NB_BYTES; i++) {
            if (i%4 == 0) cout<<" ";
            printf("%02x",key[i]);
        }
        printf("\n");
    }
}

void invert(uint8_t alt[NB_BYTES]) {
    uint8_t key[NB_BYTES];

    key[15] = alt[0];
    key[14] = alt[4];
    key[13] = alt[8];
    key[12] = alt[12];

    key[8] = alt[3] ^ key[12];
    key[11] = alt[7] ^ key[15];
    key[10] = alt[11] ^ key[14];
    key[9] = alt[15] ^ key[13];

    key[5] = alt[2] ^ key[13];
    key[4] = alt[6] ^ key[12];
    key[7] = alt[10] ^ key[15];
    key[6] = alt[14] ^ key[14];

    key[2] = alt[1] ^ key[14] ^ key[10] ^ key[6];
    key[1] = alt[5] ^ key[13] ^ key[9] ^ key[5];
    key[0] = alt[9] ^ key[12] ^ key[8] ^ key[4];
    key[3] = alt[13] ^ key[15] ^ key[11] ^ key[7];

    if (VERBOSE)
        cout<<"Inverted key"<<endl;
    for (int i = 0; VERBOSE && i < NB_BYTES; i++) {
        if (i%4 == 0) cout<<" ";
        printf("%02x",key[i]);
    }
    printf("\n");
} 

int main(int argc, char** argv) {
    if (argc < 2 || strlen(argv[1]) != 2 * NB_BYTES)
        usage(argv[0]);

    uint8_t key[NB_BYTES];

    parse_input_key(argv[1], key);

    invert(key);

    return EXIT_SUCCESS;
}