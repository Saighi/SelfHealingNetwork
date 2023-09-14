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

    int numberIterations = 10000;
    int nbPool = 50;
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
    for (int i = 0 ; i<nbPool; i++){
        vector<BinNeuron*> subPoolExcSource = WTAPools[i].Neurons;

        for (int k = 0 ; k<nbNeuronPerPool; k++){
            for (int j = 0 ; j<nbPool; j++){
                vector<BinNeuron*> subPoolExcTarget = WTAPools[j].Neurons;
                // i!=j for cross pool and not interpool
                if (i!=j){
                    // if (k==l){
                    //     double strongSynW = 0.0375/recurrentStrenghtScale; // tabula rasa start
                    //     //double strongSynW = 0.05/recurrentStrenghtScale; // engram start
                    //     crossPoolExcToExcSyn.emplace_back(strongSynW,subPoolExcI[k],subPoolExcJ[l]);
                    // }
                    // else {
                    //     double weakSynW = 0.0375/recurrentStrenghtScale; // tabula ras start 
                    //     //double weakSynW = 0.025/recurrentStrenghtScale; // engram start 
                    //     crossPoolExcToExcSyn.emplace_back(weakSynW,subPoolExcI[k],subPoolExcJ[l]);
                    // }
                    for (int l = 0; l<nbNeuronPerPool; l++){
                        // if (k==l){
                        if (k==0 && l == 0){
                            allSynapsesExc.emplace_back(2,subPoolExcSource[k],subPoolExcTarget[l]);
                        }
                        else{
                            allSynapsesExc.emplace_back(1,subPoolExcSource[k],subPoolExcTarget[l]);
                        }
                        allSynapsesInh.emplace_back(1,subPoolExcSource[k],subPoolExcTarget[l]);
                    }
                }
            }
        }
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
                    allSynExcOneInPool[i][j].addSyn(&s);
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
                    allSynInhOneInPool[i][j].addSyn(&s);
                }
            }
        }
    }

    //normalization of weights before the simulation
    for (int i = 0; i < nbPool; i++)
    {
        for (int j = 0; j < nbNeuronPerPool; j++)
        {
            allSynInhOneInPool[i][j].normalize();
        }
    }

    for (int i = 0; i < nbPool; i++)
    {
        for (int j = 0; j < nbNeuronPerPool; j++)
        {
            allSynExcOneInPool[i][j].normalize();

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

        // for (auto &s : allSynapsesInh)
        // {
        //     s.registerHistory(t);
        //     if (s.sourceNeuron->pastPastState == 1 && s.targetNeuron->pastState == 1)
        //     {
        //         s.weight += potInhib;
        //     }
        // }

        for (auto &n : binNeurons)
        {
            n.pastPastState = n.pastState;
            n.pastState = n.state;
            n.registerHistory(t);
        }
    }

    std::cout << "IS SAVING ?" << std::endl;
    // SAVING STUFF
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
