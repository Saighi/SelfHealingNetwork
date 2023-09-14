#ifndef SYNPOOL_H
#define SYNPOOL_H

#include <vector>
#include "BinNeuron.h"
#include "Synapse.h"

// Pool of all synapses connected to a neuron. Wtot could be use to normalize.

class SynPool
{
public:
    
    double wtot;
    BinNeuron* neuron;
    std::vector<Synapse*> connected_synapses;

    SynPool(double wtot, BinNeuron *neuron , std::vector<Synapse *> synapses) : wtot(wtot), neuron(neuron), connected_synapses(synapses) {}

    void normalize(){

            for (auto &s : connected_synapses)
            {
                s->weight = s->weight / wtot;
            }

            wtot = 1;
    }

    void addSyn(Synapse* syn){
        connected_synapses.push_back(syn);
        wtot += syn->weight;
    }

};

#endif
