#include <iostream>
#include <cmath>
#include <vector>
#include<tuple>
#include <random>
#include <fstream>

#include "Neuron.h"
#include "Synapse.h"
#include "ExcInhPartner.h"

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

    int poolSize = 2;
    double maxActivity = 1.;
    double autapseW = 0.05;
    int numberIterations = 100000;
    double initialActivity = 0.25;
    double lateralInhibW = 0.03;
    double inhibDecay = 0.0005;
    double inhibitoryPot = 0.005;
    double initialInhibActivity = 0;

    // Creation and initialization of the neuron pool
    vector<Neuron> excNeuronPool(poolSize, Neuron(initialActivity));
    vector<Neuron> inhNeuronPool(poolSize, Neuron(initialInhibActivity));
    vector<ExcInhPartner> excInhPartners;
    vector<Synapse> excToInhSyn;
    vector<Synapse> inhToExcSyn;
    vector<Synapse> lateralInhSyn;
    vector<Synapse> autapses;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-0.02, 0.02);

    for (size_t i = 0; i < excNeuronPool.size(); ++i) {
        Neuron* sourceNeuron = &excNeuronPool[i];
        autapses.emplace_back(autapseW,sourceNeuron,sourceNeuron); 

        for (size_t j = 0; j < excNeuronPool.size(); ++j) {
            if (i != j) {
                Neuron* targetNeuron = &excNeuronPool[j];
                lateralInhSyn.emplace_back(lateralInhibW, sourceNeuron, targetNeuron);
            }
        }
    }

    for (size_t i = 0; i < excNeuronPool.size(); ++i) {
        Neuron* neuronExc = &excNeuronPool[i];
        Neuron* neuronInh = &inhNeuronPool[i];
        excToInhSyn.emplace_back(0.0, neuronExc,neuronInh);
        inhToExcSyn.emplace_back(1.0, neuronInh,neuronExc);
    }

    for (size_t i = 0; i < excNeuronPool.size(); ++i) { 
        vector<Synapse *> affectedSynapses;
        Neuron* neuronExc = &excNeuronPool[i];
        Neuron* neuronInh = &inhNeuronPool[i];
        // cout<<"new duo"<<endl;
        for (auto& s : excToInhSyn){
            if (s.targetNeuron == neuronInh){
                affectedSynapses.emplace_back(&s);
                // cout<<neuronInh<<endl;
            }
        }
        excInhPartners.emplace_back(neuronExc,neuronInh,affectedSynapses);
    }

    // Display the synapse details
    //displaysynapsematrix(lateralInhSyn,excNeuronPool);
    //displaysynapsematrix(autapses,excNeuronPool);

    vector<vector<double>> activityHistory(poolSize, vector<double>(numberIterations));

    for(int t=0; t<numberIterations; ++t){

        // cout<<"new_iter"<<endl;
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

        for (auto& p : excInhPartners ){
            // cout<<"new duo"<<endl;
            if (p.ExcNeuron->activity>0.5){
                for (auto& s : p.affectedSynapses){
                    s->weight = s->weight+inhibitoryPot;
                    // cout<<s->sourceNeuron<<endl;
                    // cout<<s->targetNeuron<<endl;
                    // cout<<s->weight<<endl;
                }
            } 
        }

        for (size_t i = 0; i < excNeuronPool.size(); ++i) {
            Neuron& neuron = excNeuronPool[i];
            neuron.activity = min(maxActivity, max(0.0, neuron.activity + dis(gen)));
            neuron.pastActivity = neuron.activity;
            activityHistory[i][t] = neuron.activity;
        }

        for(auto& n : inhNeuronPool){
            n.pastActivity = n.activity;
            // cout<<n.activity<<endl;
        }

        // for(const auto& s : excToInhSyn){
        //     cout<<s.weight<<endl;
        // }

    }

    // Save activity history to a text file
    ofstream outputFile("activity_history.txt");
    if (outputFile.is_open()) {
        for (int i = 0; i < poolSize; ++i) {
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


