#ifndef SYNAPSE_H
#define SYNAPSE_H

#include "BinNeuron.h"

class Synapse
{
public:
    double weight;
    BinNeuron* sourceNeuron;
    BinNeuron* targetNeuron;
    vector<double> history;
    vector<int> historyTimeStamp;

    Synapse(float weight, BinNeuron* sourceNeuron, BinNeuron* targetNeuron) 
        : weight(weight), sourceNeuron(sourceNeuron), targetNeuron(targetNeuron) {}

    Synapse()
        : weight(0), sourceNeuron(nullptr), targetNeuron(nullptr) {}

    void setWeight(double newWeight){
        weight = newWeight;
    }

    
    void registerHistory(int timeStamp){
        history.push_back(weight);
        historyTimeStamp.push_back(timeStamp);
    }

};

#endif