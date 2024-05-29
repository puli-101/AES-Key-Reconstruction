#include <stdio.h>
#include <stdlib.h>
#include <aes.h>

//sample execution : ./bin/correct_bec samples/aes-128-bin_erasure.txt -v=false

void usage(char* name) {
    fprintf(stderr,"Usage : %s <filename> [options]\n", name);
    fprintf("Where <filename> contains a corrupted AES key schedule that went through the binary erasure channel\n");
    fprintf("For file formatting details, execute first ./bin/corruption <src_file> <probability> <channel_type> [options] or see ./samples/bin_erasure.txt\n");
    fprintf("\nOptions :\n-verbose : v=true | v=false\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        usage(argv[0]);
    }


    return EXIT_SUCCESS;
}