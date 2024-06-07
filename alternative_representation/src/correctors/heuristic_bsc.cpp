#include <iostream>
#include <cstdlib>
#include <queue>
#include <cstring>
#include <cmath>
#include <climits>
//#include "aes.h"
//#include "util.h"
using namespace std;

#define BLOCK_SIZE 4
#define NB_BLOCKS 4
#define MAX_SIZE 8192
#define SCALE 1
#define SUB_SCHED_SIZE (int)(BLOCK_SIZE * ROUNDS * 8 * SCALE) //size of a subschedule in bits
//Naive key reconstruction algorithm for AES-128 keys that went through the binary noisy channels
//sample execution : ./bin/correct_alt alternative/sched1_bsc_0625 0.0625 0.0625 -v=false
int CURRENT_BLOCK;
int VERBOSE = 1;

#define NB_BYTES 16
#define ROUNDS 11
#define NB_WORDS 4

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; //F

uint8_t rcon[255] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 
  0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 
  0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 
  0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 
  0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 
  0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 
  0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 
  0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 
  0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 
  0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 
  0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 
  0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 
  0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 
  0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 
  0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 
  0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb };


static inline double abs_double(double x) {
    if (x < 0)
        return -x;
    return x;
}

//prints the alternative version of the representation of the key schedule
void print_new_schedule(uint8_t s[ROUNDS][NB_BYTES]) {
    for (int i = 0; i < ROUNDS; i++) {
        for (int j = 0; j < NB_BYTES; j++) {
            if (j%4 == 0) printf(" ");
            printf("%02x",s[i][j]);
        }
        printf("\n");
    }
}

uint8_t grid[ROUNDS][NB_BYTES];       //representation ascii d'un key schedule
double proba;
double std_deviation;
double expected_value;

void correct();
int calc_diff(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int offset);
void calc_subschedule(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int index) ;

double z_score(int diff) {
    return abs_double((((double)diff) - expected_value) / std_deviation);
}

//returns the nth byte of a 4-byte word (starting from 0)
uint8_t get_byte_from_word(uint32_t w, int n) {
    return (uint8_t)(w >> (8 * n));
}

class Candidate {
private:
    uint32_t subKey;
    double score;
    int currentPosition; 
public:
    double getScore() {return score;}
    void setScore(double score) {this->score = score;}
    uint32_t getSubKey() {return subKey;}
    void setSubKey(uint32_t subKey) {this->subKey = subKey;}
    int getCurrentPosition() {return currentPosition;}
    void setCurrentPosition(int position) {this->currentPosition = position;}
    bool finishedExploring() {return currentPosition == 32;}
    void calcScore() {
        uint8_t subSchedule[ROUNDS][BLOCK_SIZE];
        for (int i = 0; i < BLOCK_SIZE; i++) {
            subSchedule[0][i] = get_byte_from_word(subKey,i);
        }
        calc_subschedule(subSchedule, CURRENT_BLOCK);
        score = z_score(calc_diff(subSchedule, CURRENT_BLOCK));
    }
};

class CompareCandidates {
public:
    bool operator() (Candidate a, Candidate b) {
        return a.getScore() > b.getScore();
    }
};


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

void parse_input(char*, int);

void correct();

void test();

int main(int argc, char** argv) {
    if (argc < 3) {
        cout<<"Usage : "<<argv[0]<<" <filename> <decay_probability> [-v=false]"<<endl;
        exit(-1);
    }

    for (int i = 3; i < argc; i++) {
        if (!strcmp(argv[i],"-v=false"))
            VERBOSE = 0;
    }

    char raw[MAX_SIZE];
    char *file = argv[1], *endPtr;
    int size = extract_text(file,raw);

    //Probability of decay from 0 to 1
    proba = strtod( argv[2], &endPtr); 
    if (endPtr == argv[2]) {
        cout<<"Format error : cannot cast third argument into double"<<endl;
    } 
    std_deviation = sqrt(SUB_SCHED_SIZE * proba * (1-proba));
    expected_value = SUB_SCHED_SIZE * proba;

    parse_input(raw, size);
    
    //Surse confirmation
    
    cout<<"Parsed input :\n";
    print_new_schedule(grid);
    cout<<"\nBruteforcing kschedule...\n";
    
    
    //test();
    correct();

    return EXIT_SUCCESS;
}

void parse_input(char* raw, int size) {
    char hex[3], *end;
    int x = 0, y = 0;

    for (int i = 0; i < size - 1; i++) {
        if (raw[i] == '\n' || raw[i] == ' ') continue;
        if (raw[i] == '\0' || raw[i] == EOF) break;
        hex[0] = raw[i];
        hex[1] = raw[i+1];
        hex[2] = '\0';
        grid[x][y] = strtol(hex, &end, 16); 
        if (end == hex) {
            cout<<"ERROR"<<endl;
        }
        y++;
        if (y >= NB_BYTES) {
            y = 0;
            x++;

            if (x > ROUNDS) {
                cout<<"ERROR"<<endl;
            }
        } 
        i++;
    }
}

uint32_t byteArrayInto32_t(uint8_t array[4]) {
    uint32_t res = 0;
    for (int i = 0; i < 4; i++) {
        res |= ((uint32_t)array[i]) << (i * 8);
    }
    return res;
}

//prints a progress bar on screen
void print_progress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

void correct() {
    for (int i = 0; i < 4; i++) {
        cout<<"Solving block "<<i<<endl;
        CURRENT_BLOCK = i;
        priority_queue<Candidate, vector<Candidate>, CompareCandidates> q;
        Candidate first;
        u_int32_t count = 0;
        u_int8_t firstVector[4];
        
        for (int j = 0; j < 4; j++) {
            firstVector[j] = grid[0][j + 4 * CURRENT_BLOCK];
            //cout<<hex<<unsigned(firstVector[j])<<dec<<' ';
        }

        bool found = false;
        double prcntg = 0.0;

        first.setCurrentPosition(-1);
        first.setSubKey(byteArrayInto32_t(firstVector));
        first.calcScore();
        q.push(first);

        int threshold = ((double)UINT32_MAX * 0.0125);

        //cout<<endl<<"First subKey to be analyzed: "<<hex<<first.getSubKey()<<dec<<endl;
        while(!q.empty()) {
            if ((count % threshold == 0)) {
                print_progress(prcntg);
                prcntg += 0.0125;
            }
            Candidate c = q.top();
            q.pop();

            /*cout<<"Candidate "<<count<<endl;
            cout<<"Key "<<hex<<c.getSubKey()<<dec<<endl;
            cout<<"Score "<<c.getScore()<<endl;
            cout<<"Position "<<c.getCurrentPosition()<<endl<<endl;
            */
            if (c.getScore() < (1.5 * SCALE)) {
                found = true;
                print_progress(1);
                cout<<endl<<endl<<"Found after "<<count<<" iterations !\n->";
                for (int i = 0 ; i < 4; i++)
                    printf("%02x ",get_byte_from_word(c.getSubKey(),i));
                cout<<"\nWith a score of "<<c.getScore()<<endl<<endl;
                break;
            }
            if (c.finishedExploring())
                continue;
            
            Candidate sub;
            sub.setCurrentPosition(c.getCurrentPosition() + 1);
            sub.setSubKey(c.getSubKey() ^ (1 << sub.getCurrentPosition()));
            sub.calcScore();
            q.push(sub);

            c.setCurrentPosition(c.getCurrentPosition() + 1);
            q.push(c);
            count++;
        }

        if (!found)
            cout<<endl<<"Key Not found after "<<count<<" iterations !\n";
    }
}

void calc_subschedule(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int index) {
    for (int i = 1; i < ROUNDS; i++) {
        subschedule[i][0] = subschedule[i-1][1] ^ sbox[subschedule[i-1][0]];
        subschedule[i][1] = subschedule[i-1][2];
        subschedule[i][2] = subschedule[i-1][3];
        subschedule[i][3] = subschedule[i-1][0];
        if (index % 4 == 2) {
            subschedule[i][0] ^= rcon[i];
        }
        index = (index + 1) % 4;
    }
    
}

//Calculates the hamming distance between a subschedule 
//i.e. the slice of an alternative key schedule generated by 4 contiguous bytes, and
//the corresponding subschedule from the originally extracted key schedule 
//index : {0,1,2,3} refers to the word number in the first round of the alternative schedule
int calc_diff(uint8_t subschedule[ROUNDS][BLOCK_SIZE], int offset) {
    int diff = 0, delta = 0;
    uint8_t byte_diff;

    for (int i = 0; i < ((int)((float)ROUNDS * SCALE)); i++) {
        delta = 0;
        for (int j = 0; j < BLOCK_SIZE; j++) {
            byte_diff = subschedule[i][j] ^ grid[i][(j + 4*(offset+i))%NB_BYTES];
            for (int k = 0; k < 8; k++) {
                if ( (1 << k) & byte_diff)
                    delta++;
            }
        }
        diff += delta;
    }
    return diff;
}