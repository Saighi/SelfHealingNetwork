#ifndef BINNEURON_H
#define BINNEURON_H

#include <vector>

using namespace std;

class BinNeuron
{
private :

    static int nextId;
    int id;

public:

    int state;
    int pastState;
    int pastPastState;
    vector<int> history;
    vector<int> historyTimeStamp;

    BinNeuron(){
        id = nextId++;
        state = 0;
        pastState = 0;
        pastPastState =0;
    }

    int getID() const {
        return id;
    }

    void SetState(int newState){
        state = newState;
    }

    // Doesn't have any time stamp, should/could maybe change 
    void registerHistory(int timeStamp){
        history.push_back(state);
        historyTimeStamp.push_back(timeStamp);
    }
};

#endif