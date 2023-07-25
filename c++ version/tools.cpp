
#include <iostream>
#include <iomanip>
#include <vector>
#include "Neuron.h"
#include "Synapse.h"

using namespace std;

void displaysynapsematrix(const vector<Synapse>& synapses, const vector<Neuron>& neuronPoolSource, const vector<Neuron>& neuronPoolTarget) {
    // Display column headers
    cout << "   ";
    for (size_t i = 0; i < neuronPoolTarget.size(); ++i) {
        cout << i << "\t";
    }
    cout << endl;

    // Display rows
    for (size_t i = 0; i < neuronPoolSource.size(); ++i) {
        // Display row header (source neuron index)
        cout << i << "  ";

        // Display synapse weights
        for (size_t j = 0; j < neuronPoolTarget.size(); ++j) {
            // Find the synapse linking the source and target neurons
            const Synapse* synapse = nullptr;
            for (const auto& s : synapses) {
                if (s.sourceNeuron == &neuronPoolSource[i] && s.targetNeuron == &neuronPoolTarget[j]) {
                    synapse = &s;
                    break;
                }
            }

            // Display synapse weight
            if (synapse != nullptr) {
                cout << synapse->weight << setprecision(2)<<fixed<< "\t";
            } else {
                cout << "0\t";
            }
        }
        cout << endl;
    }
}
