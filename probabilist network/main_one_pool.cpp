#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <random>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "BinNeuron.h"
#include "Synapse.h"
#include "SubPool.h"
#include "SynPool.h"

using namespace std;
int BinNeuron::nextId = 1;

struct SYNHIST{
    Synapse* syn;
    vector<double> history;
    void writeHist(){
        history.push_back((syn->weight));
    }
};

struct NEURONHIST{
    BinNeuron* neuron;
    vector<int> history;
    void writeHist(){
        history.push_back((neuron->state));
    }
};

int main(int argc, char **argv) {

    int numberIterations = 1000;
    int nbPool = 1;
    int nbNeuronPerPool = 2;
    int TimeOn = 2;
    int nbNeurons = nbPool*nbNeuronPerPool;
    
    vector<BinNeuron> binNeurons(nbNeurons); 
    vector<SubPool> WTAPools; 
    vector<Synapse> allSynapsesExc;
    vector<Synapse> allSynapsesInh;
    vector<vector<SynPool>> allSynExcOneInPool;
    vector<vector<SynPool>> allSynInhOneInPool;

    random_device rd;
    mt19937 gen(rd());

    // Forming different pools of neurons with WTA dynamics intra pools.
    for(int i =0; i<nbPool; i++){
       vector<BinNeuron*> subPoolExc(nbNeuronPerPool);
        for(int j = 0; j<nbNeuronPerPool;j++){
            subPoolExc[j]=&binNeurons[(i*nbNeuronPerPool)+j];
        } 
        WTAPools.emplace_back(subPoolExc);
    }

    // Initializing the vectors that will later gather all the recurrent synapses inputing a given neuron.
    for (int i = 0; i<nbPool;i++){
        vector<SynPool> allSynExcOne;
        for(int j = 0; j<nbNeuronPerPool; j++){
            vector<Synapse*> emptySynVec;
            allSynExcOne.emplace_back(0, &binNeurons[(i*nbNeuronPerPool)+j],emptySynVec);
        }
        allSynExcOneInPool.push_back(allSynExcOne);
    }

    for (int i = 0; i<nbPool;i++){
        vector<SynPool> allSynInhOne;
        for(int j = 0; j<nbNeuronPerPool; j++){
            vector<Synapse*> emptySynVec;
            allSynInhOne.emplace_back(0, &binNeurons[(i*nbNeuronPerPool)+j],emptySynVec);
        }
        allSynInhOneInPool.push_back(allSynInhOne);
    }

    // Adding recurrent excitatory and inhibitory synapses between neurons of different pools
    for (int i = 0 ; i<nbPool; i++){
        vector<BinNeuron*> subPoolExcSource = WTAPools[i].Neurons;

        for (int k = 0 ; k<nbNeuronPerPool; k++){
            for (int j = 0 ; j<nbPool; j++){
                vector<BinNeuron*> subPoolExcTarget = WTAPools[j].Neurons;
                double meanWeight = (nbPool-1)*nbNeurons;
                // i!=j for cross pool and not interpool
                if (i!=j){
                    for (int l = 0; l<nbNeuronPerPool; l++){
                            allSynapsesExc.emplace_back(meanWeight,subPoolExcSource[k],subPoolExcTarget[l]);
                            allSynapsesInh.emplace_back(0,subPoolExcSource[k],subPoolExcTarget[l]);
                    }
                }
            }
        } 
    }

    // Gathering all the recurrent excitatory synapses inputing a neuron.
    for(auto& s : allSynapsesExc){
        for (int i = 0; i<nbPool;i++){
            vector<SynPool> allSynExcOne = allSynExcOneInPool[i];
            for (int j =0; j<nbNeuronPerPool;j++){
                if (WTAPools[i].Neurons[j] == s.targetNeuron){
                    allSynExcOne[j].connected_synapses.push_back(&s);
                }


            }
        }
    }

    // Gathering all the recurrent inhibitory synapses inputing a neuron.
    for(auto& s : allSynapsesInh){
        for (int i = 0; i<nbPool;i++){
            vector<SynPool> allSynInhOne = allSynInhOneInPool[i];
            for (int j =0; j<nbNeuronPerPool;j++){
                if (WTAPools[i].Neurons[j] == s.targetNeuron){
                    allSynInhOne[j].connected_synapses.push_back(&s);
                }


            }
        }
    }

    // Inhibited stimulation protocol stuff down there :
    vector<BinNeuron> stimNodes(1); // wont be affected by the network dynamic, are juste placeholders inducing a stimulation.
    stimNodes[0].state = 1;
    stimNodes[0].pastState = 1;
    stimNodes[0].pastPastState = 1;


    //Neuron activity in time
    vector<NEURONHIST> excNeuronHist(binNeurons.size());
    for(int i = 0 ; i<binNeurons.size(); i++){
        excNeuronHist[i].neuron= &binNeurons[i];
        excNeuronHist[i].history= vector<int>();
    }

    vector<SYNHIST> allSynapsesExcHist(allSynapsesExc.size());
    for(int i = 0 ; i<allSynapsesExc.size(); i++){
        allSynapsesExcHist[i].syn= &allSynapsesExc[i];
        allSynapsesExcHist[i].history= vector<double>();
    }

    vector<SYNHIST> allSynapsesInhHist(allSynapsesInh.size());
    for(int i = 0 ; i<allSynapsesInh.size(); i++){
        allSynapsesInhHist[i].syn= &allSynapsesInh[i];
        allSynapsesInhHist[i].history= vector<double>();
    }

    for(int t=0; t<numberIterations; ++t){

    }

    return 0;
}
