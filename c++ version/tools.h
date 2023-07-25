#ifndef TOOLS_H
#define TOOLS_H

#include <vector>
#include "Synapse.h"
#include "Neuron.h" 

void displaysynapsematrix(const std::vector<Synapse>& synapses, const std::vector<Neuron>& neuronPoolSource, const std::vector<Neuron>& neuronPoolTarget);

#endif 