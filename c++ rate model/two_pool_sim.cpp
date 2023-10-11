#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <random>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "Neuron.h"
#include "Synapse.h"

using namespace std;
int Neuron::nextId = 1;

enum PoolType
{
    square,
    hexagon
};

double transferFunc(double inputCurrent)
{
    return 1 / (1 + exp(-inputCurrent));
}

int main(int argc, const char **argv)
{

    // Simulation and network structure constants
    const int nbIter = 100;
    const int nbPool = 2;
    const PoolType poolType = square;
    int nbNeurons;
    if (poolType == square)
    {
        nbNeurons = 4 * nbPool;
    }
    // synapses constants
    const double hebPotExc = 0.001;
    const double hebPotInh = 0.01;
    const double decayInh = 0.001;
    // neuron constants
    const double noise = 0.05;
    double targetInhibActivity = 0.2;     // will change to induce oscillations, should stay low in the exponential regim
    const double targetExcActivity = 0.4; // such that even with noise the activity is not lower in the exponential regim that inhibition

    // Creation and initialization of the neuron pool
    vector<Neuron> excNeurons(nbNeurons);
    vector<Neuron> inhNeurons(nbNeurons);

    return 0;
}