#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <random>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <numeric> // Include the numeric library for std::accumulate
#include <string>

#include "BinNeuron.h"
#include "Synapse.h"
#include "SubPool.h"
#include "SynPool.h"

using namespace std;
int BinNeuron::nextId = 1;

// TO REMOVE To put history into synapses
// struct SYNHIST{
//     Synapse* syn;
//     vector<double> history;
//     void writeHist(){
//         history.push_back((syn->weight));
//     }
// };

int main(int argc, char **argv)
{

    int numberIterations = 500000;
    int nbPool = 1;
    int nbNeuronPerPool = 2;
    vector<int> timesOn = {1, 2, 3};
    vector<double> probabilityOn = {3.0 / 6.0, 2.0 / 6.0, 1.0 / 6.0};
    int nbNeurons = nbPool * nbNeuronPerPool;
    double potInhib = 0.00005;

    vector<BinNeuron> binNeurons(nbNeurons);
    vector<SubPool> WTAPools;
    vector<Synapse> allSynapsesExc;
    vector<Synapse> allSynapsesInh;
    vector<vector<SynPool>> allSynExcOneInPool;
    vector<vector<SynPool>> allSynInhOneInPool;

    random_device rd;
    mt19937 gen(rd());

    // Forming different pools of neurons with WTA dynamics intra pools.
    for (int i = 0; i < nbPool; i++)
    {
        vector<BinNeuron *> subPoolExc(nbNeuronPerPool);
        for (int j = 0; j < nbNeuronPerPool; j++)
        {
            subPoolExc[j] = &binNeurons[(i * nbNeuronPerPool) + j];
        }
        WTAPools.emplace_back(subPoolExc);
    }

    // Initializing the vectors that will later gather all the recurrent synapses inputing a given neuron.
    for (int i = 0; i < nbPool; i++)
    {
        vector<SynPool> allSynExcOne;
        for (int j = 0; j < nbNeuronPerPool; j++)
        {
            vector<Synapse *> emptySynVec;
            allSynExcOne.emplace_back(0, &binNeurons[(i * nbNeuronPerPool) + j], emptySynVec);
        }
        allSynExcOneInPool.push_back(allSynExcOne);
    }

    for (int i = 0; i < nbPool; i++)
    {
        vector<SynPool> allSynInhOne;
        for (int j = 0; j < nbNeuronPerPool; j++)
        {
            vector<Synapse *> emptySynVec;
            allSynInhOne.emplace_back(0, &binNeurons[(i * nbNeuronPerPool) + j], emptySynVec);
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
    allSynapsesExc.emplace_back(0.25, &stimNodes[0], WTAPools[0].Neurons[0]);
    allSynapsesExc.emplace_back(0.75, &stimNodes[0], WTAPools[0].Neurons[1]);

    allSynapsesExc.emplace_back(0.75, &stimNodes[1], WTAPools[0].Neurons[0]);
    allSynapsesExc.emplace_back(0.25, &stimNodes[1], WTAPools[0].Neurons[1]);

    allSynapsesInh.emplace_back(0.5, &stimNodes[0], WTAPools[0].Neurons[0]);
    allSynapsesInh.emplace_back(0.5, &stimNodes[0], WTAPools[0].Neurons[1]);

    allSynapsesInh.emplace_back(0.5, &stimNodes[1], WTAPools[0].Neurons[0]);
    allSynapsesInh.emplace_back(0.5, &stimNodes[1], WTAPools[0].Neurons[1]);

    vector<vector<int>> stimNodesActivity;
    int nbStepOnOff = 15;
    int cpt;

    for (int i = 0; i < nbStimNode; i++)
    {
        vector<int> activity(numberIterations);
        cpt = 0;
        for (int j = 0; j < activity.size(); j++)
        {
            if (i == 0)
            {
                // activity[j] = 1;
                activity[j] = (cpt / nbStepOnOff) % 2;
            }
            else
            {
                // activity[j]= 0; //TO CHANGE !!!
                activity[j] = (1 - ((cpt / nbStepOnOff) % 2));
            }
            cpt++;
        }
        stimNodesActivity.push_back(activity);
    }

    // Gathering all the recurrent excitatory synapses inputing a neuron.
    for (auto &s : allSynapsesExc)
    {
        for (int i = 0; i < nbPool; i++)
        {
            for (int j = 0; j < nbNeuronPerPool; j++)
            {
                if (WTAPools[i].Neurons[j] == s.targetNeuron)
                {
                    allSynExcOneInPool[i][j].connected_synapses.push_back(&s);
                    allSynExcOneInPool[i][j].wtot += s.weight;
                }
            }
        }
    }

    // Gathering all the recurrent inhibitory synapses inputing a neuron.
    for (auto &s : allSynapsesInh)
    {
        for (int i = 0; i < nbPool; i++)
        {
            for (int j = 0; j < nbNeuronPerPool; j++)
            {
                if (WTAPools[i].Neurons[j] == s.targetNeuron)
                {
                    allSynInhOneInPool[i][j].connected_synapses.push_back(&s);
                    allSynInhOneInPool[i][j].wtot += s.weight;
                }
            }
        }
    }

    double sumExc;
    double sumInh;
    double sumNormalization;
    vector<double> activityIndice(nbNeuronPerPool);
    vector<double> normalizedActivityIndice(nbNeuronPerPool);
    std::discrete_distribution<int> distribution;
    double sumActivityIndice;
    // vector<int> indexNeuronInPool(nbNeuronPerPool);
    // iota(indexNeuronInPool.begin(),indexNeuronInPool.end(),0); // fill the indexNeuronInPool with values going from 0 -> nbNeuronPerPool
    int randomIndex;
    int randomTimeOnIndex;
    double diff;

    for (int t = 0; t < numberIterations; t++)
    {

        stimNodes[0].pastPastState = stimNodes[0].pastState;
        stimNodes[1].pastPastState = stimNodes[1].pastState;
        stimNodes[0].pastState = stimNodes[0].state;
        stimNodes[1].pastState = stimNodes[1].state;
        stimNodes[0].state = stimNodesActivity[0][t];
        stimNodes[1].state = stimNodesActivity[1][t];

        for (int i = 0; i < nbPool; i++)
        {
            for (int j = 0; j < nbNeuronPerPool; j++)
            {
                diff = (allSynInhOneInPool[i][j].wtot - 1) / allSynInhOneInPool[i][j].connected_synapses.size();
                for (auto &s : allSynExcOneInPool[i][j].connected_synapses)
                {
                    s->weight = s->weight / allSynExcOneInPool[i][j].wtot;
                }
                for (auto &s : allSynInhOneInPool[i][j].connected_synapses)
                {
                    // s->weight = s->weight / allSynInhOneInPool[i][j].wtot;
                    s->weight = s->weight - diff;
                }
            }
        }

        for (int i = 0; i < nbPool; i++)
        {

            if (WTAPools[i].timeOn > 0)
            {
                WTAPools[i].timeOn -= 1;
            }

            if (WTAPools[i].timeOn == 0)
            {

                for (int j = 0; j < nbNeuronPerPool; j++)
                {
                    vector<Synapse *> coSynExc = allSynExcOneInPool[i][j].connected_synapses;
                    sumExc = 0;
                    sumNormalization = 0;
                    for (auto &s : coSynExc)
                    {
                        sumExc += s->weight * s->sourceNeuron->pastState;
                        sumNormalization += s->weight;
                    }
                    allSynExcOneInPool[i][j].wtot = sumNormalization;

                    vector<Synapse *> coSynInh = allSynInhOneInPool[i][j].connected_synapses;
                    sumInh = 0;
                    sumNormalization = 0;
                    for (auto &s : coSynInh)
                    {
                        sumInh += s->weight * s->sourceNeuron->pastPastState;
                        sumNormalization += s->weight;
                    }
                    allSynInhOneInPool[i][j].wtot = sumNormalization;

                    activityIndice[j] = min(1.0, max(sumExc / sumInh, 0.0001));
                }

                // for (auto& indice: activityIndice)
                // {
                //     cout<<indice<<endl;
                // }

                sumActivityIndice = accumulate(activityIndice.begin(), activityIndice.end(), 0.0);
                // std::cout << "sumActivityIndice" << std::endl;
                // std::cout << sumActivityIndice << std::endl;
                for (int j = 0; j < nbNeuronPerPool; j++)
                {
                    normalizedActivityIndice[j] = activityIndice[j] / sumActivityIndice;
                }

                // std::cout << "normalized activity indice" << std::endl;
                // for (auto& indice: normalizedActivityIndice)
                // {
                //     cout<<indice<<endl;
                // }

                distribution = discrete_distribution<int>(normalizedActivityIndice.begin(), normalizedActivityIndice.end());
                randomIndex = distribution(gen);
                for (size_t k = 0; k < nbNeuronPerPool; k++)
                {
                    if (k == randomIndex)
                    {
                        WTAPools[i].Neurons[k]->SetState(1);
                    }
                    else
                    {
                        WTAPools[i].Neurons[k]->SetState(0);
                    }
                }

                distribution = discrete_distribution<int>(probabilityOn.begin(), probabilityOn.end());
                randomTimeOnIndex = distribution(gen);
                WTAPools[i].timeOn = timesOn[randomTimeOnIndex];

                // std::cout << "WTAPools[i].timeOn" << std::endl;
                // std::cout << WTAPools[i].timeOn << std::endl;
                // std::cout << "New state :" << std::endl;
                // std::cout << WTAPools[i].Neurons[0]->state << std::endl;
                // std::cout << WTAPools[i].Neurons[1]->state << std::endl;
            }
        }

        for (auto &s : allSynapsesInh)
        {
            s.registerHistory(t);
            if (s.sourceNeuron->pastPastState == 1 && s.targetNeuron->pastState == 1)
            {
                s.weight += potInhib;
            }
        }

        for (auto &n : binNeurons)
        {
            n.pastPastState = n.pastState;
            n.pastState = n.state;
            n.registerHistory(t);
        }
    }

    int nbOnesOnOnes = 0;
    int nbZerosOnOnes = 0;
    int nbOnesOnZeros = 0;
    int nbZerosOnZeros = 0;
    BinNeuron n = binNeurons[0];

    for (int i = 0; i < n.history.size(); i++)
    {
        if (stimNodesActivity[0][n.historyTimeStamp[i]] == 1)
        {
            if (n.history[i] == 0)
            {
                nbZerosOnOnes += 1;
            }
            else
            {
                nbOnesOnOnes += 1;
            }
        }
    }

    // std::cout << "numberOfOnes" << std::endl;
    // std::cout << nbZerosOnOnes << std::endl;
    // std::cout << "numberOfZeros" << std::endl;
    // std::cout << nbOnesOnOnes << std::endl;
    std::cout << "IS SAVING ?" << std::endl;
    // SAVING STUFF
    // if stimulation :
    ofstream outputFileStimNode("activity_stim_node.txt");
    ofstream outputFileStimNode2("activity_stim_node_2.txt");
    if (outputFileStimNode.is_open() && outputFileStimNode2.is_open())
    {
        string to_writeSt;
        string to_writeSt2;
        for (int i = 0; i < stimNodesActivity[0].size(); i++)
        {
            to_writeSt += to_string(stimNodesActivity[0][i]) + "\n";
            to_writeSt2 += to_string(stimNodesActivity[1][i]) + "\n";
        }
        outputFileStimNode << to_writeSt << endl;
        outputFileStimNode2 << to_writeSt2 << endl;
        outputFileStimNode.close();
        outputFileStimNode2.close();
    }

    // universal
    for (auto &n : binNeurons)
    {
        std::cout << n.getID() << std::endl;

        string folderName = "neuron_" + to_string(n.getID());
        filesystem::path folderPath = folderName;
        // Create the folder
        if ((std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) || std::filesystem::create_directory(folderPath))
        {

            std::cout << "Neuron folder created successfully." << std::endl;

            // Save activity history to a text file
            ofstream outputFile(folderName + "/activity.txt");
            ofstream outputFile2(folderName + "/activity_time_stamp.txt");
            if (outputFile.is_open() && outputFile2.is_open())
            {
                string to_write;
                string to_write2;
                for (int i = 0; i < n.history.size(); ++i)
                {
                    to_write += to_string(n.history[i]) + "\n";
                    to_write2 += to_string(n.historyTimeStamp[i]) + "\n";
                }
                outputFile << to_write << endl;
                outputFile2 << to_write2 << endl;
                outputFile.close();
                outputFile2.close();
            }
            else
            {
                cerr << "Error: Unable to open the output file." << endl;
            }

            // Save coming synapses excitatory weights

            filesystem::path folderPath2 = folderName + "/coming_exc_syn";

            if ((std::filesystem::exists(folderPath2) && std::filesystem::is_directory(folderPath2)) || std::filesystem::create_directory(folderPath2))
            {

                std::cout << "Coming recurrent excitatory synapse folder created successfully." << std::endl;

                ofstream outputFile(folderName + "/coming_exc_syn/weight_evolution.txt");
                ofstream outputFile2(folderName + "/coming_exc_syn/weight_evolution_time_stamp.txt");

                if (outputFile.is_open() && outputFile2.is_open())
                {
                    for (auto &s : allSynapsesExc)
                    {
                        if (s.targetNeuron->getID() == n.getID())
                        {
                            string line;
                            string line2;
                            for (int i = 0; i < s.history.size(); i++)
                            {
                                line += to_string(s.history[i]) + "\t";
                                line2 += to_string(s.historyTimeStamp[i]) + "\t";
                            }
                            line.pop_back();
                            line2.pop_back();
                            outputFile << line << endl;
                            outputFile2 << line2 << endl;
                        }
                    }
                    outputFile.close();
                    outputFile2.close();
                }
            }
            else
            {
                std::cerr << "Failed to create folder." << std::endl;
            }

            // Save coming synapses inhibitory weights

            folderPath2 = folderName + "/coming_inh_syn";

            if ((std::filesystem::exists(folderPath2) && std::filesystem::is_directory(folderPath2)) || std::filesystem::create_directory(folderPath2))
            {

                std::cout << "Coming recurrent inhibitory synapse folder created successfully." << std::endl;

                ofstream outputFile(folderName + "/coming_inh_syn/weight_evolution.txt");
                ofstream outputFile2(folderName + "/coming_inh_syn/weight_evolution_time_stamp.txt");

                if (outputFile.is_open() && outputFile2.is_open())
                {
                    for (auto &s : allSynapsesInh)
                    {
                        if (s.targetNeuron->getID() == n.getID())
                        {
                            string line;
                            string line2;
                            for (int i = 0; i < s.history.size(); i++)
                            {
                                line += to_string(s.history[i]) + "\t";
                                line2 += to_string(s.historyTimeStamp[i]) + "\t";
                            }
                            line.pop_back();
                            line2.pop_back();
                            outputFile << line << endl;
                            outputFile2 << line2 << endl;
                        }
                    }
                    outputFile.close();
                    outputFile2.close();
                }
            }
            else
            {
                std::cerr << "Failed to create folder." << std::endl;
            }
        }
        else
        {
            std::cerr << "Failed to create folder." << std::endl;
        }
    }

    return 0;
}
