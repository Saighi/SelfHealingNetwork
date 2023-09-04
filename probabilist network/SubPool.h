
#ifndef SUBPOOL_H
#define SUBPOOL_H

#include <vector>
#include "BinNeuron.h"

// SubPools are pool of excitatory and inhibitory neurons with a specific connectivity 
// inside the pool and between pools. Usually, if lateral inhibition is strong anough, we 
// get a winner takes all dynamic. 

class SubPool
{
public:
    
    std::vector<BinNeuron*> Neurons;
    int timeOn;

    SubPool(std::vector<BinNeuron *> Neurons) : Neurons(Neurons) {
        timeOn=0;
    }

};

#endif