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
#include "ExcInhPartner.h"
#include "SubPool.h"
#include "SynPool.h"
#include "tools.h"

using namespace std;
int Neuron::nextId = 1;

struct SYNHIST{
    Synapse* syn;
    vector<double> history;
    void writeHist(){
        history.push_back((syn->weight));
    }
};

struct NEURONHIST{
    Neuron* neuron;
    vector<double> history;
    void writeHist(){
        history.push_back((neuron->activity));
    }
};

int main(int argc, char **argv) {
    // when changing nbPool and nbNeuronPerPool we need to scale the recurrent excitatory synaptic weights, the noise and the excSpontaneousFiring
    int numberIterations = 200000;
    int nbPool = 10;
    int nbNeuronPerPool = 6;
    int nbNeurons = nbPool*nbNeuronPerPool;
    double maxActivity = 100;
    double autapseW = 0.00;
    double initialActivity = 0.25;
    double lateralInhibW = 0.08;
    double inhibDecay = 0.0004;
    double inhibitoryPot = 0.04;
    double initialInhibActivity = 0;
    double hebbian_pot = 0.0001;
    //double excSpontaneousFiring = 0.02; // reduce time between spontaneous emergence of assembly. (internal system pressure)
    double excSpontaneousFiring = 0.02/1.5; // reduce time between spontaneous emergence of assembly. (internal system pressure)
    double noiseIntensity = 0.02/1.5;
    double recurrentStrenghtScale = 1.2; //change the normalized total synaptic weight.
    double lateralInhibitionPlasticityTreshold = 0.15;
    double potLateralInhibition = inhibitoryPot;
    double decayLateralInhibition = inhibDecay;
    double spontHebbianPot = 0.00006;
    double crossPoolInhibTreshold = 2;
    double crossPoolHebbianTreshold = 1.6;

    // Creation and initialization of the neuron pool
    vector<Neuron> excNeurons(nbNeurons);
    for (auto& n : excNeurons){
        n.activity=initialActivity;
        n.pastActivity=initialActivity;
    }
    vector<Neuron> inhNeurons(nbNeurons);
    for (auto& n : inhNeurons){
        n.activity=initialInhibActivity;
        n.pastActivity=initialInhibActivity;
    }

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

    // vector which will be shuffle and copied to choose which pool target which pool later in the code.
    vector<int> integerRange(nbPool);

    for (int i = 0; i < nbPool; i++){
        integerRange[i] = i;
    }

    // We just fill with empty vectors allSynOneExc
    for(int i = 0; i<nbNeurons; i++){
        vector<Synapse*> emptySynVec;
        allSynOneExc.emplace_back(0, &excNeurons[i],emptySynVec);
    }

    // Forming different pools of neurons with WTA dynamics intra pools.
    for(int i =0; i<nbPool; i++){
       vector<Neuron*> subPoolExc(nbNeuronPerPool);
       vector<Neuron*> subPoolInh(nbNeuronPerPool);
        for(int j = 0; j<nbNeuronPerPool;j++){
            subPoolExc[j]=&excNeurons[(i*nbNeuronPerPool)+j];
            subPoolInh[j]=&inhNeurons[(i*nbNeuronPerPool)+j];
        } 
        subPools.emplace_back(subPoolExc,subPoolInh);
    }

    // Lateral inhibition inside each pool
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

    // Cross pool synapses
    // Cross pool synapses are forming assemblies with strong synapses where 
    // each neuron in the assembly is strongly excitated by each other neurons of the assembly and inhibited by every neurons
    // for better realism and (maybe) inducing cool emergent dynamics we need TODO :
    // TODO : each neuron is excitated by a random half of the pools and inhibited by the other half.
    
    for (int i = 0 ; i<nbPool; i++){
        vector<Neuron*> subPoolExcI = subPools[i].ExcNeurons;
        vector<Neuron*> subPoolInhI = subPools[i].InhNeurons;

        // Vector to shuffle to know which pool is connected with which pool through inhibitory and/or excitatory connections
        vector<int> to_shuffle(integerRange);
        shuffle(to_shuffle.begin(),to_shuffle.end(), gen);
        vector<int> recur_exc_pools(to_shuffle.begin(),to_shuffle.begin()+(int)(nbPool/2));

        for (int k = 0 ; k<nbNeuronPerPool; k++){
            for (int j = 0 ; j<nbPool; j++){
                vector<Neuron*> subPoolExcJ = subPools[j].ExcNeurons;
                // i!=j for cross pool and not interpool
                if (i!=j){
                    for (int l = 0; l<nbNeuronPerPool; l++){

                        excToInhSyn.emplace_back(0, subPoolExcJ[l],subPoolInhI[k]);

                        if (k==l){
                            double strongSynW = 0.0375/recurrentStrenghtScale; // tabula rasa start
                            //double strongSynW = 0.05/recurrentStrenghtScale; // engram start
                            crossPoolExcToExcSyn.emplace_back(strongSynW,subPoolExcI[k],subPoolExcJ[l]);
                        }
                        else {
                            double weakSynW = 0.0375/recurrentStrenghtScale; // tabula ras start 
                            //double weakSynW = 0.025/recurrentStrenghtScale; // engram start 
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

    vector<Neuron*> cluster; 
    neuronClusters.push_back(cluster);

    for (int i = 0; i < nbPool; i++)
    {
        int randomInt = int((dis(gen)+noiseIntensity)/(noiseIntensity*2)*nbNeuronPerPool);
        neuronClusters[0].push_back(subPools[i].ExcNeurons[randomInt]);
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

    //Neuron activity in time
    vector<NEURONHIST> excNeuronHist(excNeurons.size());
    for(int i = 0 ; i<excNeurons.size(); i++){
        excNeuronHist[i].neuron= &excNeurons[i];
        excNeuronHist[i].history= vector<double>();
    }

    //vector<vector<vector<double>>> activityHistoryClusters(neuronClusters.size(), vector<vector<double>>(nbNeurons,vector<double>(numberIterations)));
    double sum_w;

    // Synapses in time
    vector<SYNHIST> lateralHinibSynHist(lateralInhSyn.size());
    for(int i = 0 ; i<lateralInhSyn.size(); i++){
        lateralHinibSynHist[i].syn= &lateralInhSyn[i];
        lateralHinibSynHist[i].history= vector<double>();
    }

    vector<SYNHIST> recExcSynHist(crossPoolExcToExcSyn.size());
    for(int i = 0 ; i<crossPoolExcToExcSyn.size(); i++){
        recExcSynHist[i].syn= &crossPoolExcToExcSyn[i];
        recExcSynHist[i].history= vector<double>();
    }

    vector<SYNHIST> recExcToInhSynHist(excToInhSyn.size());
    for(int i = 0 ; i<excToInhSyn.size(); i++){
        recExcToInhSynHist[i].syn= &excToInhSyn[i];
        recExcToInhSynHist[i].history= vector<double>();
    }

    // Stimulation parameters :

    bool stim = false;
    int t_start = numberIterations/2 ;
    int t_stop = numberIterations/2 + 1000;
    int clusterToStim = 0;
    double strengthStim = 0.5;
    int nbNeuronToStim = nbPool;

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
            s.targetNeuron->activity= s.targetNeuron->activity+(s.sourceNeuron->pastActivity*s.weight);
        }

        for (auto &s : lateralInhSyn){
            s.targetNeuron->activity = s.targetNeuron->activity-(s.sourceNeuron->pastActivity*s.weight);
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
            s.targetNeuron->activity= s.targetNeuron->activity-(s.sourceNeuron->pastActivity*s.weight);
        }

        for (auto& s : crossPoolExcToExcSyn) {
            s.targetNeuron->activity= s.targetNeuron->activity+(s.sourceNeuron->pastActivity*s.weight);
            if (s.sourceNeuron->pastActivity>crossPoolHebbianTreshold && s.targetNeuron->pastActivity>crossPoolHebbianTreshold){
                s.weight+=hebbian_pot;
            }
            s.weight+=spontHebbianPot*s.sourceNeuron->pastActivity*s.targetNeuron->pastActivity;
        }

        for (auto& p : excInhPartners){
            if (p.ExcNeuron->pastActivity>crossPoolInhibTreshold){
                for (auto& s : p.affectedSynapses){
                    s->weight = s->weight+(inhibitoryPot*s->sourceNeuron->pastActivity);
                }
            } 
        }

        for (auto& neuron : excNeurons) {
            neuron.activity = min(maxActivity, max(0.0, neuron.activity + dis(gen)+excSpontaneousFiring));
            neuron.pastActivity = neuron.activity;
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
                    //activityHistoryClusters[i][j][t] = n->pastActivity;
                    if(i == clusterToStim && t>t_start && t<t_stop && j<nbNeuronToStim){
                        n->activity = min(1.0,n->activity+strengthStim);
                    }
                }
            }
        }

        // writing the history of each synapses (could be done with a method if synHist was an object)
        for(auto& synHist : lateralHinibSynHist){
            synHist.writeHist();
        }

        for(auto& synHist : recExcSynHist){
            synHist.writeHist();
        }

        for(auto& neuronHist : excNeuronHist){
            neuronHist.writeHist();
        }

        // if (t==0){
        //     displaysynapsematrix(crossPoolExcToExcSyn,excNeurons, excNeurons);
        // }
        // if (t==numberIterations-1){
        //     displaysynapsematrix(crossPoolExcToExcSyn,excNeurons, excNeurons);
        // }
        //displaysynapsematrix(excToInhSyn,excNeurons, inhNeurons);
    }

    for(auto& n : excNeurons){

        string folderName = "neuron_"+to_string(n.getID());
        filesystem::path folderPath = folderName;
        // Create the folder
        if ((std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) || std::filesystem::create_directory(folderPath)) {

            std::cout << "Neuron folder created successfully." << std::endl;

            // Save activity history to a text file
            ofstream outputFile(folderName+"/activity.txt");
            if (outputFile.is_open()) {
                    for (auto& neuronHist : excNeuronHist){
                        if (neuronHist.neuron->getID()==n.getID()){
                            string to_write;
                            for (int t = 0; t < numberIterations; ++t) {
                                to_write += to_string(neuronHist.history[t])+"\n";
                            }
                            outputFile << to_write << endl;
                        }
                    }
                    outputFile.close();
            } else {
                cerr << "Error: Unable to open the output file." << endl;
            }

            //Save coming synapses weights

            filesystem::path folderPath2 = folderName+"/coming_exc_syn";

            if ((std::filesystem::exists(folderPath2) && std::filesystem::is_directory(folderPath2)) || std::filesystem::create_directory(folderPath2)) {

                std::cout << "Coming recurrent excitatory synapse folder created successfully." << std::endl;

                ofstream outputFile(folderName+"/coming_exc_syn/activity.txt");
                if (outputFile.is_open()){
                    for (auto& h : recExcSynHist){
                        if (h.syn->targetNeuron->getID() == n.getID()){
                            string line;
                            for(int t = 0; t < numberIterations; t++){
                                line += to_string(h.history[t]) + "\t";
                            }
                            line.pop_back();
                            outputFile << line <<endl;
                        }

                    }
                    outputFile.close();
                }

            } else {
                std::cerr << "Failed to create folder." << std::endl;
            }

            folderPath2 = folderName+"/coming_lateral_inh_syn";

            if ((std::filesystem::exists(folderPath2) && std::filesystem::is_directory(folderPath2)) || std::filesystem::create_directory(folderPath2)) {

                std::cout << "Coming lateral inhibitory synapse folder created successfully." << std::endl;

                ofstream outputFile(folderName+"/coming_lateral_inh_syn/activity.txt");
                if (outputFile.is_open()){
                    for (auto& h : lateralHinibSynHist){
                        if (h.syn->targetNeuron->getID() == n.getID()){
                            string line;
                            for(int t = 0; t < numberIterations; t++){
                                line += to_string(h.history[t]) + "\t";
                            }
                            line.pop_back();
                            outputFile << line <<endl;
                        }

                    }
                    outputFile.close();
                }

            } else {
                std::cerr << "Failed to create folder." << std::endl;
            }

        } else {
            std::cerr << "Failed to create folder." << std::endl;
        }
    }

    return 0;
}

