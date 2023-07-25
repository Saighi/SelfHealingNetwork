#include <iostream>
#include <cmath>
#include <vector>
#include<tuple>
#include <random>
#include <fstream>

#include "Neuron.h"
#include "Synapse.h"
#include "ExcInhPartner.h"
#include "SubPool.h"
#include "SynPool.h"
#include "tools.h"

using namespace std;

int main(int argc, char **argv) {
    // when changing nbPool and nbNeuronPerPool we need to scale the recurrent excitatory synaptic weights, the noise and the excSpontaneousFiring
    int nbPool = 10;
    int nbNeuronPerPool = 3;
    int nbNeurons = nbPool*nbNeuronPerPool;
    double maxActivity = 1.5;
    double autapseW = 0.00;
    int numberIterations = 50000;
    double initialActivity = 0.25;
    double lateralInhibW = 0.08;
    double inhibDecay = 0.002;
    double inhibitoryPot = 0.02;
    double initialInhibActivity = 0;
    double hebbian_pot = 0.0001;
    //double excSpontaneousFiring = 0.02; // reduce time between spontaneous emergence of assembly. (internal system pressure)
    double excSpontaneousFiring = 0.02/3; // reduce time between spontaneous emergence of assembly. (internal system pressure)
    double noiseIntensity = 0.02/3;
    double recurrentStrenghtScale = 5; //change the normalized total synaptic weight.
    double lateralInhibitionPlasticityTreshold = 0.1;
    double potLateralInhibition = inhibitoryPot;
    double decayLateralInhibition = inhibDecay;

    // Creation and initialization of the neuron pool
    vector<Neuron> excNeurons(nbNeurons, Neuron(initialActivity));
    vector<Neuron> inhNeurons(nbNeurons, Neuron(initialInhibActivity));
    vector<SubPool> subPools; 
    vector<ExcInhPartner> excInhPartners;
    vector<Synapse> excToInhSyn;
    vector<Synapse> inhToExcSyn;
    vector<Synapse> lateralInhSyn;
    vector<Synapse> autapses;
    vector<Synapse> crossPoolExcToExcSyn;
    vector<SynPool> allSynOneExc;
    vector<vector <Neuron*>> neuronClusters;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-noiseIntensity, noiseIntensity);

    for(int i = 0; i<nbNeurons; i++){
        vector<Synapse*> emptySynVec;
        allSynOneExc.emplace_back(0, &excNeurons[i],emptySynVec);
    }

    for(int i =0; i<nbPool; i++){
       vector<Neuron*> subPoolExc(nbNeuronPerPool);
       vector<Neuron*> subPoolInh(nbNeuronPerPool);
        for(int j = 0; j<nbNeuronPerPool;j++){
            subPoolExc[j]=&excNeurons[(i*nbNeuronPerPool)+j];
            subPoolInh[j]=&inhNeurons[(i*nbNeuronPerPool)+j];
        } 
        subPools.emplace_back(subPoolExc,subPoolInh);
    }

    for (auto& pool : subPools){

       vector<Neuron*> subPoolExc = pool.ExcNeurons;
       vector<Neuron*> subPoolInh = pool.InhNeurons;

        for (size_t i = 0; i < subPoolExc.size(); ++i) {
            Neuron* sourceNeuron = subPoolExc[i];
            //autapses.emplace_back(autapseW,sourceNeuron,sourceNeuron); 

            for (size_t j = 0; j < subPoolExc.size(); ++j) {
                if (i != j) {
                    Neuron* targetNeuron = subPoolExc[j];
                    lateralInhSyn.emplace_back(lateralInhibW, sourceNeuron, targetNeuron);
                }
            }
        }

        for (size_t i = 0; i < subPoolExc.size(); ++i) {
            Neuron* neuronExc = subPoolExc[i];
            Neuron* neuronInh = subPoolInh[i];
            excToInhSyn.emplace_back(0.0, neuronExc,neuronInh);
            inhToExcSyn.emplace_back(1.0, neuronInh,neuronExc);
        }


    }

    // Cross pool synapses
    for (int i = 0 ; i<nbPool; i++){
        vector<Neuron*> subPoolExcI = subPools[i].ExcNeurons;
        vector<Neuron*> subPoolInhI = subPools[i].InhNeurons;
        for (int k = 0 ; k<nbNeuronPerPool; k++){
            for (int j = 0 ; j<nbPool; j++){
                vector<Neuron*> subPoolExcJ = subPools[j].ExcNeurons;
                // i!=j for cross pool and not interpool
                if (i!=j){
                    for (int l = 0; l<nbNeuronPerPool; l++){

                        excToInhSyn.emplace_back(0, subPoolExcJ[l],subPoolInhI[k]);
                        // cout<<subPoolExcJ[l]<<endl;
                        // cout<<subPoolInhI[k]<<endl;
                        //crossPoolExcToExcSyn 
                        if (k==l){
                            // double strongSynW = 0.0375/recurrentStrenghtScale; // tabula rasa start
                            double strongSynW = 0.05/recurrentStrenghtScale; // engram start
                            crossPoolExcToExcSyn.emplace_back(strongSynW,subPoolExcI[k],subPoolExcJ[l]);
                        }
                        else {
                            // double weakSynW = 0.0375/recurrentStrenghtScale; // tabula ras start 
                            double weakSynW = 0.025/recurrentStrenghtScale; // engram start 
                            crossPoolExcToExcSyn.emplace_back(weakSynW,subPoolExcI[k],subPoolExcJ[l]);
                        }


                    }

                }
            } 
        }
    }

    // gathering neuron clusters
    // for (int k = 0 ; k<nbNeuronPerPool; k++){
    //     vector<Neuron*> cluster; 
    //     neuronClusters.push_back(cluster);
    // }

    for (size_t i = 0; i < nbPool; i++)
    {
        int randomInt = int((dis(gen)+noiseIntensity)/(noiseIntensity*2)*nbPool);
        std::cout << randomInt << std::endl;
    }
    

    // for (int k = 0 ; k<nbNeuronPerPool; k++){ 
    //     for (int i = 0 ; i<nbPool; i++){
    //         neuronClusters[k].push_back(subPools[i].ExcNeurons[k]);
    //     }
    // }
    
    // for(auto& c: neuronClusters){
    //     std::cout << "new cluster:" << std::endl;
    //     for(auto& n : c){
    //         cout<<n<<endl;
    //     }
    // }

    for(auto& s : crossPoolExcToExcSyn){
        for(int m =0; m<nbNeurons; m++){
            if (allSynOneExc[m].neuron==s.targetNeuron){
                allSynOneExc[m].wtot += crossPoolExcToExcSyn.back().weight;
                allSynOneExc[m].connected_synapses.push_back(&s);
                // Add the last created recurrent excitatory to excitatory synapse to the right target neuron 
                // in the allSynOneExc vector of object to allow normalization of those synaptic weights later.
            }
        }
    }
    // I do a second loop over the pools to make the inhibitory and excitatory partners
    // either it messes up with the pointers idk why.
    // Personal note :Maybe because we are pointing toward the adress in excToInhSyn and when we pushback
    // we are modifying which element is at which adress.
    // Personal Remainder : always mess with pointers of a vector after finishing initializing it with all objects.
    for (auto& pool : subPools){

       vector<Neuron*> subPoolExc = pool.ExcNeurons;
       vector<Neuron*> subPoolInh = pool.InhNeurons;

        for (size_t i = 0; i < subPoolExc.size(); ++i) { 
            vector<Synapse *> affectedSynapses;
            Neuron* neuronExc = subPoolExc[i];
            Neuron* neuronInh = subPoolInh[i];
            for (auto& s : excToInhSyn){
                if (s.targetNeuron == neuronInh){
                    affectedSynapses.push_back(&s);
                }
            }
            ExcInhPartner newPartner(neuronExc,neuronInh,affectedSynapses);
            excInhPartners.push_back(newPartner);
        }

    }

    // Display the synapse details
    //displaysynapsematrix(crossPoolExcToExcSyn,excNeurons, excNeurons);
    // displaysynapsematrix(lateralInhSyn,excNeurons);
    //displaysynapsematrix(autapses,excNeuronPool);
    vector<vector<double>> activityHistory(nbNeurons, vector<double>(numberIterations));
    double sum_w;

    // Stimulation parameters :

    bool stim = false;
    int t_start = numberIterations/2 ;
    int t_stop = numberIterations/2 + 1000;
    int clusterToStim = 0;
    double strengthStim = 0.2;
    int nbNeuronToStim = 2;

    // SIMULATION :
    for(int t=0; t<numberIterations; ++t){

        // Inhibitory neurons don't have memory contrary to excitatory neurons that keeps
        // their activity mimicking the slow dynamics of nmda and the highly recurrent aspect
        // of excitatory activity compared to fast changing and short term memory of inhibition.
        for(auto& n : inhNeurons){
            n.activity = 0;
        }

        //cout<<"new_iter"<<endl;
        for (auto& s : autapses) {
            s.targetNeuron->activity= min(maxActivity,s.targetNeuron->activity+(s.sourceNeuron->pastActivity*s.weight));
        }

        for (auto &s : lateralInhSyn){
            s.targetNeuron->activity = max(0.0,s.targetNeuron->activity-(s.sourceNeuron->pastActivity*s.weight));
            if(s.sourceNeuron->pastActivity>lateralInhibitionPlasticityTreshold && s.targetNeuron->pastActivity>lateralInhibitionPlasticityTreshold){
                s.weight+= potLateralInhibition;
            }
            else{
                s.weight = max(0.0, s.weight-decayLateralInhibition);
            }
        }

        for (auto& s : excToInhSyn) {
            // cout<<s.weight<<endl;
            s.targetNeuron->activity += s.sourceNeuron->pastActivity*s.weight;
            s.weight = max(0.0,s.weight-inhibDecay);
        }

        for (auto& s : inhToExcSyn) {
            s.targetNeuron->activity= max(0.0, s.targetNeuron->activity-(s.sourceNeuron->pastActivity*s.weight));
        }

        for (auto& s : crossPoolExcToExcSyn) {
            s.targetNeuron->activity= min(maxActivity,s.targetNeuron->activity+(s.sourceNeuron->pastActivity*s.weight));
            if (s.sourceNeuron->pastActivity>0.3 && s.targetNeuron->pastActivity>0.3){
                s.weight+=hebbian_pot;
            }
        }

        for (auto& p : excInhPartners){
            if (p.ExcNeuron->pastActivity>0.5){
                for (auto& s : p.affectedSynapses){
                    s->weight = s->weight+(inhibitoryPot*s->sourceNeuron->pastActivity);
                }
            } 
        }

        for (size_t i = 0; i < excNeurons.size(); ++i) {
            Neuron& neuron = excNeurons[i];
            neuron.activity = min(maxActivity, max(0.0, neuron.activity + dis(gen)+excSpontaneousFiring));
            neuron.pastActivity = neuron.activity;
            // cout<<activityHistory[i].size()<<endl;
            activityHistory[i][t] = neuron.activity;
        }

        for(auto& n : inhNeurons){
            n.pastActivity = n.activity;
            // cout<<n.activity<<endl;
        }

        // NORMALIZATION

        for(auto& p : allSynOneExc){

            sum_w = 0;
            
            for (auto& s1 : p.connected_synapses){
                sum_w += s1->weight;
            }

            for (auto& s : p.connected_synapses){
                s->weight = s->weight*(p.wtot/sum_w);
            }

        }

        // Stimulation :
        if (stim){
            for (int i = 0; i < neuronClusters.size(); i++)
            {
                for (int j = 0 ; j <neuronClusters[i].size() ; j++)
                {
                    Neuron* n = neuronClusters[i][j];
                    if(i == clusterToStim && t>t_start && t<t_stop && j<nbNeuronToStim){
                        n->activity = min(1.0,n->activity+strengthStim);
                    }
                }
            }
        }

        // if (t==0){
        //     displaysynapsematrix(crossPoolExcToExcSyn,excNeurons, excNeurons);
        // }
        // if (t==numberIterations-1){
        //     displaysynapsematrix(crossPoolExcToExcSyn,excNeurons, excNeurons);
        // }
        //displaysynapsematrix(excToInhSyn,excNeurons, inhNeurons);
    }

    double sum;
    for(auto& p : allSynOneExc){
        sum =0;
        for (auto& s :p.connected_synapses){
            sum+=s->weight;
        }
        // cout<<sum<<endl;
        // cout<<p.wtot<<endl;
    }

    // Save activity history to a text file
    ofstream outputFile("activity_history.txt");
    if (outputFile.is_open()) {
        for (int i = 0; i < nbNeurons; ++i) {
            string line;
            for (int t = 0; t < numberIterations; ++t) {
                line += to_string(activityHistory[i][t]) + "\t";
            }
            line.pop_back(); // Remove the last '\t'
            outputFile << line << endl;
        }
        outputFile.close();
    } else {
        cerr << "Error: Unable to open the output file." << endl;
    }

    return 0;
}


