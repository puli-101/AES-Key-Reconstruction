#include <iostream>
#include <cstdlib>
#include <queue>
#include <cstring>
#include <cmath>
#include <climits>
#include "aes.h"
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

static inline double abs_double(double x) {
    if (x < 0)
        return -x;
    return x;
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