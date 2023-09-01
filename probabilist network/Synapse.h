#ifndef SYNAPSE_H
#define SYNAPSE_H

#include "BinNeuron.h"

class Synapse
{
public:
    double weight;
    BinNeuron* sourceNeuron;
    BinNeuron* targetNeuron;

    Synapse(float weight, BinNeuron* sourceNeuron, BinNeuron* targetNeuron) 
        : weight(weight), sourceNeuron(sourceNeuron), targetNeuron(targetNeuron) {}

    Synapse()
        : weight(0), sourceNeuron(nullptr), targetNeuron(nullptr) {}
};

#endif