#include <iostream>
#include <cstdlib>
#include <queue>
#include <cstring>
#include <cmath>
#include <climits>
#include "aes.h"
#include "util.h"
#include "resolution_handler.h"
using namespace std;

int CURRENT_BLOCK;

#define NB_BYTES 16
#define ROUNDS 11
#define NB_WORDS 4

uint8_t grid[ROUNDS][NB_BYTES];      
double proba;
double std_deviation;
double expected_value;

void correct();

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
        score = z_score(calc_diff(subSchedule, grid, CURRENT_BLOCK), expected_value, std_deviation);
    }
};

class CompareCandidates {
public:
    bool operator() (Candidate a, Candidate b) {
        return a.getScore() > b.getScore();
    }
};

void parse_input(char*, int);

void correct();

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
    
    //confirmation
    cout<<"Parsed input :\n";
    print_new_schedule(grid);
    cout<<"\nBruteforcing kschedule...\n";
    
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

