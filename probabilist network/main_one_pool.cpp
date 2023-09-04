#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <random>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <numeric> // Include the numeric library for std::accumulate

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
    vector<int> timesOn = {1,2,3};
    vector<double> probabilityOn = {3/6,2/6,1/6};
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
    // for (int i = 0 ; i<nbPool; i++){
    //     vector<BinNeuron*> subPoolExcSource = WTAPools[i].Neurons;

    //     for (int k = 0 ; k<nbNeuronPerPool; k++){
    //         for (int j = 0 ; j<nbPool; j++){
    //             vector<BinNeuron*> subPoolExcTarget = WTAPools[j].Neurons;
    //             double meanWeight = (nbPool-1)*nbNeurons;
    //             // i!=j for cross pool and not interpool
    //             if (i!=j){
    //                 for (int l = 0; l<nbNeuronPerPool; l++){
    //                         allSynapsesExc.emplace_back(meanWeight,subPoolExcSource[k],subPoolExcTarget[l]);
    //                         allSynapsesInh.emplace_back(meanWeight,subPoolExcSource[k],subPoolExcTarget[l]);
    //                 }
    //             }
    //         }
    //     } 
    // }

    // Inhibited stimulation protocol stuff down there :
    int nbStimNode = 2;
    vector<BinNeuron> stimNodes(nbStimNode); // wont be affected by the network dynamic, are juste placeholders inducing a stimulation.
    allSynapsesExc.emplace_back(0.5,&stimNodes[0],WTAPools[0].Neurons[0]);
    allSynapsesExc.emplace_back(0.5,&stimNodes[0],WTAPools[0].Neurons[1]);

    allSynapsesExc.emplace_back(0.5,&stimNodes[1],WTAPools[0].Neurons[0]);
    allSynapsesExc.emplace_back(0.5,&stimNodes[1],WTAPools[0].Neurons[1]);

    allSynapsesInh.emplace_back(0.5,&stimNodes[0],WTAPools[0].Neurons[0]);
    allSynapsesInh.emplace_back(0.5,&stimNodes[0],WTAPools[0].Neurons[1]);

    allSynapsesInh.emplace_back(0.5,&stimNodes[1],WTAPools[0].Neurons[0]);
    allSynapsesInh.emplace_back(0.5,&stimNodes[1],WTAPools[0].Neurons[1]);

    vector<vector<int>> stimNodesActivity;
    int nbStepOnOff = 10;
    int cpt;

    for (int i = 0; i<nbStimNode;i++){
        vector<int> activity(numberIterations);
        cpt = 0;
        for (int j = 0; j<activity.size(); j++){
            if (i ==0){
                activity[j]=(cpt/nbStepOnOff)%2;
            }
            else{
                activity[j]=(1-((cpt/nbStepOnOff)%2));
            }
            cpt++;
        }
        stimNodesActivity.push_back(activity);
    }

    // Gathering all the recurrent excitatory synapses inputing a neuron.
    for(auto& s : allSynapsesExc){
        for (int i = 0; i<nbPool;i++){
            for (int j =0; j<nbNeuronPerPool;j++){
                if (WTAPools[i].Neurons[j] == s.targetNeuron){
                    allSynExcOneInPool[i][j].connected_synapses.push_back(&s);
                }


            }
        }
    }

    // Gathering all the recurrent inhibitory synapses inputing a neuron.
    for(auto& s : allSynapsesInh){
        for (int i = 0; i<nbPool;i++){
            for (int j =0; j<nbNeuronPerPool;j++){
                if (WTAPools[i].Neurons[j] == s.targetNeuron){
                    allSynInhOneInPool[i][j].connected_synapses.push_back(&s);
                }
            }
        }
    }

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

    double sumExc;
    double sumInh;
    vector<double> activityIndice(nbNeuronPerPool);
    vector<double> normalizedActivityIndice(nbNeuronPerPool);
    std::discrete_distribution<int> distribution;
    double sumActivityIndice;
    // vector<int> indexNeuronInPool(nbNeuronPerPool);
    // iota(indexNeuronInPool.begin(),indexNeuronInPool.end(),0); // fill the indexNeuronInPool with values going from 0 -> nbNeuronPerPool
    int randomIndex;
    int randomTimeOnIndex; 

    for (int t=0; t<numberIterations; ++t){

        stimNodes[0].state=stimNodesActivity[0][t];
        stimNodes[1].state=stimNodesActivity[1][t];

        for (int i=0; i<nbPool;i++){

            if (WTAPools[i].timeOn>0){
                WTAPools[i].timeOn -= 1;
            }
            
            if (WTAPools[i].timeOn==0){

                for (int j =0; j<nbNeuronPerPool; j++){
                    
                    vector<Synapse*> coSynExc = allSynExcOneInPool[i][j].connected_synapses;
                    sumExc = 0;
                    for (auto& s: coSynExc){
                        sumExc+=s->weight*s->sourceNeuron->pastState;
                    }

                    vector<Synapse*> coSynInh = allSynInhOneInPool[i][j].connected_synapses;
                    sumInh = 0;
                    for (auto& s: coSynInh){
                        sumInh+=s->weight*s->sourceNeuron->pastPastState;
                    }

                    activityIndice[j] = sumExc/sumInh;
                }

                sumActivityIndice = accumulate(activityIndice.begin(),activityIndice.begin(),0.0);
                for (int j=0;j<nbNeuronPerPool;j++){
                    normalizedActivityIndice[j] = activityIndice[j]/sumActivityIndice;
                }

                distribution = discrete_distribution<int>(normalizedActivityIndice.begin(), normalizedActivityIndice.end());
                randomIndex = distribution(gen);
                for (size_t k = 0; k < nbNeuronPerPool; k++)
                {
                    if ( k == randomIndex){
                        WTAPools[i].Neurons[k]->state=1; 
                    }
                    else{
                        WTAPools[i].Neurons[k]->state=0; 
                    }
                }

                distribution = discrete_distribution<int>(probabilityOn.begin(), probabilityOn.end());
                randomTimeOnIndex = distribution(gen);
                WTAPools[i].timeOn=timesOn[randomTimeOnIndex]; 

            }
        }

        for (auto& n: binNeurons){
            n.pastPastState = n.pastState;
            n.pastState = n.state;
        }
    }

    return 0;
}
