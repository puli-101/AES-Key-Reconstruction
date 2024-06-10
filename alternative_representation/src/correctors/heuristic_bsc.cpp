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

uint8_t grid[ROUNDS][NB_BYTES];      
double proba;
double std_deviation;
double expected_value;

void correct();

//Representation of a subKey candidate
class Candidate {
private:
    uint32_t subKey;        //all 32 bits of the candidate subkey
    double score;           //z score of candidate
    int currentPosition;    //from 0 to 32
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

//Class used to sort candidates
class CompareCandidates {
public:
    bool operator() (Candidate a, Candidate b) {
        return a.getScore() > b.getScore();
    }
};

void correct();

int main(int argc, char** argv) {
    if (argc < 3) {
        cout<<"Usage : "<<argv[0]<<" <filename> <decay_probability> [-v=false]"<<endl;
        exit(-1);
    }

    //Option handling
    for (int i = 3; i < argc; i++) {
        if (!strcmp(argv[i],"-v=false"))
            VERBOSE = 0;
    }

    //Extraction of noisy key schedule
    char raw[MAX_SIZE];
    char *file = argv[1], *endPtr;
    int size = extract_text(file,raw);

    //Extraction of probability of decay of the channel
    proba = strtod( argv[2], &endPtr); 
    if (endPtr == argv[2]) {
        cout<<"Format error : cannot cast third argument into double"<<endl;
    } 
    std_deviation = sqrt(SUB_SCHED_SIZE * proba * (1-proba));
    expected_value = SUB_SCHED_SIZE * proba;

    //Parsing and parsing confirmation confirmation
    parse_input(raw, size, grid);
    cout<<"Parsed input :\n";
    print_new_schedule(grid);
    cout<<"\n(Presque) Bruteforcing kschedule...\n";
    
    correct();

    return EXIT_SUCCESS;
}

void correct() {
    //We correct each of the 4 32-bit blocks making up the key
    for (int i = 0; i < 4; i++) {
        cout<<"Solving block "<<i<<endl;
        CURRENT_BLOCK = i;
        priority_queue<Candidate, vector<Candidate>, CompareCandidates> q;  //keps track of the closest candidate to an actual solution
        Candidate first;            //first tested candidate
        u_int8_t firstVector[4];    //first vector = extracted noisy subkey that corresponds to the i-th block 
        
        for (int j = 0; j < NB_BLOCKS; j++) {
            firstVector[j] = grid[0][j + 4 * CURRENT_BLOCK];
        }

        //initialization of all fields of first tested subkey
        first.setCurrentPosition(-1);
        first.setSubKey(byteArrayInto32_t(firstVector));
        first.calcScore();
        q.push(first);

        int threshold = ((double)UINT32_MAX * 0.0125);  //threshold for updating progress bar on screen
        bool found = false;         //determines if an ideal candidate has been found
        double prcntg = 0.0;        //keeps track of percentage of tested candidates
        u_int32_t count = 0;        //keeps track of number of tested candidates  

        while(!q.empty()) {
            if ((count % threshold == 0)) {
                print_progress(prcntg);
                prcntg += 0.0125;
            }
            Candidate c = q.top();
            q.pop();

            //If the candidate's score is close enough to the expected number of errors
            //then we print it on screen
            if (c.getScore() < (1.5 * SCALE)) {
                found = true;
                print_progress(1);
                cout<<endl<<endl<<"Found after "<<count<<" iterations !\n->";
                for (int i = 0 ; i < NB_BLOCKS; i++)
                    printf("%02x ",get_byte_from_word(c.getSubKey(),i));
                cout<<"\nWith a score of "<<c.getScore()<<endl<<endl;
                break;
            }
            if (c.finishedExploring())
                continue;
            
            //Calculates two new candidates
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

