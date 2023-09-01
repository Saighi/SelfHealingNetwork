#ifndef EXCINHPARTNER_H
#define EXCINHPARTNER_H

#include <vector>

#include "Neuron.h"
#include "Synapse.h"

// Excitatory neurons are linked to inhibitory synapses 
// such that some plasticity rules of inhibitory synapses 
// depends on excitatory activity.

// Note : this is a non trivial behavior for which I use this
// ad hoc formalism, there is maybe a better way to do it.

class ExcInhPartner 
{
public:
    
    Neuron *ExcNeuron;
    Neuron *InhNeuron;
    std::vector<Synapse *> affectedSynapses;

    ExcInhPartner(Neuron *ExcNeuron, Neuron *InhNeuron, std::vector<Synapse *> affectedSynapses) : ExcNeuron(ExcNeuron),InhNeuron(InhNeuron), affectedSynapses(affectedSynapses) {}

};

#endif