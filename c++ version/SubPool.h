
#ifndef SUBPOOL_H
#define SUBPOOL_H

#include <vector>
#include "Neuron.h"

// SubPools are pool of excitatory and inhibitory neurons with a specific connectivity 
// inside the pool and between pools. Usually, if lateral inhibition is strong anough, we 
// get a winner takes all dynamic. 

class SubPool
{
public:
    
    std::vector<Neuron*> ExcNeurons;
    std::vector<Neuron*> InhNeurons;

    SubPool(std::vector<Neuron *> ExcNeurons, std::vector<Neuron *> InhNeurons) : ExcNeurons(ExcNeurons),InhNeurons(InhNeurons) {}

};

#endif