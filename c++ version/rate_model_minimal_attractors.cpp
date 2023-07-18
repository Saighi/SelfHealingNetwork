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

using namespace std;void displaysynapsematrix(const vector<Synapse>& synapses, const vector<Neuron>& neuronPool) {
    // Display column headers
    cout << "   ";
    for (size_t i = 0; i < neuronPool.size(); ++i) {
        cout << i << "\t";
    }
    cout << endl;

    // Display rows
    for (size_t i = 0; i < neuronPool.size(); ++i) {
        // Display row header (source neuron index)
        cout << i << "  ";

        // Display synapse weights
        for (size_t j = 0; j < neuronPool.size(); ++j) {
            // Find the synapse linking the source and target neurons
            const Synapse* synapse = nullptr;
            for (const auto& s : synapses) {
                if (s.sourceNeuron == &neuronPool[i] && s.targetNeuron == &neuronPool[j]) {
                    synapse = &s;
                    break;
                }
            }

            // Display synapse weight
            if (synapse != nullptr) {
                cout << synapse->weight << "\t";
            } else {
                cout << "0\t";
            }
        }
        cout << endl;
    }
}


int main(int argc, char **argv) {
    int nbPool = 2;
    int nbNeuronPool = 2;
    int nbNeurons = nbPool*nbNeuronPool;
    double maxActivity = 1.;
    double autapseW = 0.05;
    int numberIterations = 1000;
    double initialActivity = 0.25;
    double lateralInhibW = 0.03;
    double inhibDecay = 0.0005;
    double inhibitoryPot = 0.005;
    double initialInhibActivity = 0;

    // Creation and initialization of the neuron pool
    vector<Neuron> excNeurons(nbNeurons, Neuron(initialActivity));
    vector<Neuron> inhNeurons(nbNeurons, Neuron(initialInhibActivity));
    vector<SubPool> subPools; 
    vector<ExcInhPartner> excInhPartners;
    vector<Synapse> excToInhSyn;
    vector<Synapse> inhToExcSyn;
    vector<Synapse> lateralInhSyn;
    vector<Synapse> autapses;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-0.02, 0.02);

    for(int i =0; i<nbPool; i++){
       vector<Neuron*> subPoolExc(nbNeuronPool);
       vector<Neuron*> subPoolInh(nbNeuronPool);
        for(int j = 0; j<nbNeuronPool;j++){
            subPoolExc[j]=&excNeurons[(i*nbNeuronPool)+j];
            subPoolInh[j]=&inhNeurons[(i*nbNeuronPool)+j];
        } 
        subPools.emplace_back(subPoolExc,subPoolInh);
    }

    for (auto& pool : subPools){

       vector<Neuron*> subPoolExc = pool.ExcNeurons;
       vector<Neuron*> subPoolInh = pool.InhNeurons;

        for (size_t i = 0; i < subPoolExc.size(); ++i) {
            Neuron* sourceNeuron = subPoolExc[i];
            autapses.emplace_back(autapseW,sourceNeuron,sourceNeuron); 

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

    // I do a second loop over the pools to make the inhibitory and excitatory partners
    // either it messes up with the pointers idk why.
    // Personal note :Maybe because we are pointing toward the adress in excToInhSyn and when we pushback
    // we are modifying which element is at which adress.
    // Personal Remainder : always mess with pointers of a vector after finishing initializing it with all objects.
    for (auto& pool : subPools){

       vector<Neuron*> subPoolExc = pool.ExcNeurons;
       vector<Neuron*> subPoolInh = pool.InhNeurons;

        for (size_t i = 0; i < subPoolExc.size(); ++i) { 
            vector<Synapse *> affectedSynapses(1);
            Neuron* neuronExc = subPoolExc[i];
            Neuron* neuronInh = subPoolInh[i];
            for (auto& s : excToInhSyn){
                if (s.targetNeuron == neuronInh){
                    cout<<s.sourceNeuron<<endl;
                    cout<<s.targetNeuron<<endl;
                    affectedSynapses[0]=&s;
                }
            }
            ExcInhPartner newPartner(neuronExc,neuronInh,affectedSynapses);
            excInhPartners.push_back(newPartner);
        }

    }
    // Display the synapse details
    //displaysynapsematrix(lateralInhSyn,excNeuronPool);
    //displaysynapsematrix(autapses,excNeuronPool);
    vector<vector<double>> activityHistory(nbNeurons, vector<double>(numberIterations));

    for(int t=0; t<numberIterations; ++t){

        //cout<<"new_iter"<<endl;
        for (auto& s : autapses) {
            s.targetNeuron->activity= min(maxActivity,s.targetNeuron->activity+(s.targetNeuron->pastActivity*s.weight));
        }

        for (auto &s : lateralInhSyn){
            s.targetNeuron->activity = max(0.0,s.targetNeuron->activity-(s.sourceNeuron->pastActivity*s.weight));
        }

        for (auto& s : excToInhSyn) {
            s.targetNeuron->activity = s.sourceNeuron->pastActivity*s.weight;
            s.weight = max(0.0,s.weight-inhibDecay);
        }

        for (auto& s : inhToExcSyn) {
            s.targetNeuron->activity= max(0.0, s.targetNeuron->activity-(s.sourceNeuron->pastActivity*s.weight));
        }

        for (auto& p : excInhPartners){
            if (p.ExcNeuron->activity>0.5){
                for (auto& s : p.affectedSynapses){
                    s->weight = s->weight+inhibitoryPot;
                    // cout<<s->sourceNeuron<<endl;
                    // cout<<s->targetNeuron<<endl;
                    // cout<<s->weight<<endl;
                }
            } 
        }

        for (size_t i = 0; i < excNeurons.size(); ++i) {
            Neuron& neuron = excNeurons[i];
            neuron.activity = min(maxActivity, max(0.0, neuron.activity + dis(gen)));
            neuron.pastActivity = neuron.activity;
            activityHistory[i][t] = neuron.activity;
        }

        for(auto& n : inhNeurons){
            n.pastActivity = n.activity;
            // cout<<n.activity<<endl;
        }

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


