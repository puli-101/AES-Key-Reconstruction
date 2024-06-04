#include "util.h"

int VERBOSE = 1;


//prints a progress bar on screen
void print_progress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

//returns the nth byte of a 4-byte word (starting from 0)
uint8_t get_byte_from_word(uint32_t w, int n) {
    return (uint8_t)(w >> (8 * n));
}

//print usage header
void print_header(char* bin_name, char* format) {
    print_color(stderr,"Input Formatting Error","red",'\n');
    print_color(stderr, "Usage :","yellow",' ');
    print_color(stderr,bin_name, "cyan",' ');
    print_color(stderr,format, "cyan", '\n');
}

//sets a specific color for printing on terminal
void set_color(FILE* output, char* color) {
    char* buff = get_color(color);
    fprintf(output,"%s",buff);
    free(buff);
}

//prints a message msg with a certain color and a specific end character
void print_color(FILE* output, char* msg, char* color, char end) {
    set_color(output, color);
    fprintf(output, "%s%c",msg,end);
    set_color(output, "default");
}

//returns a specific color for printing on terminal
char* get_color(char* color) {
    if (!strcmp(color,"red")) {
        return strdup("\033[1;31m"); 
    } else if (!strcmp(color,"yellow")) {
        return strdup("\033[1;33m"); 
    } else if (!strcmp(color, "cyan")) {
        return strdup("\033[0;36m"); 
    } else if (!strcmp(color,"green")) {
        return strdup("\033[0;32m"); 
    }
    return strdup("\033[0m");
}

//extracts all bytes of a text and stores it in a buffer
//returns the size of the textfile
int extract_text(char* file, char* buffer) {
    int length;
    FILE* f = fopen(file, "r");
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    fread (buffer, sizeof(char), length, f);
    fclose(f);
    return length;
}

//xor of a and b where a and b are characters in {'0','1'}
char ascii_xor(char a, char b) {
    if ((a == '0' || a == '1') && (b == '0' || b == '1'))
        return ((a == '1' && b == '1') || (a == '0' && b == '0')) ? '0' : '1';
    else
        fprintf(stderr,"Error while x-oring a : %c , b : %c \n",a,b);
    exit(EXIT_FAILURE);
}