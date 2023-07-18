#ifndef SYNAPSE_H
#define SYNAPSE_H

#include "Neuron.h"

class Synapse
{
public:
    double weight;
    Neuron* sourceNeuron;
    Neuron* targetNeuron;

    Synapse(float weight, Neuron* sourceNeuron, Neuron* targetNeuron) 
        : weight(weight), sourceNeuron(sourceNeuron), targetNeuron(targetNeuron) {}

    Synapse()
        : weight(0), sourceNeuron(nullptr), targetNeuron(nullptr) {}
};

#endif