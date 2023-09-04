#ifndef BINNEURON_H
#define BINNEURON_H

class BinNeuron
{
private :

    static int nextId;
    int id;

public:
    
    double inhibActivity;
    double state;
    double pastState;
    double pastPastState;

    BinNeuron(){

        inhibActivity = 0;
        id = nextId++;
        state = 0;
        pastState = 0;
        pastPastState =0;
    }

    int getID() const {
        return id;
    }
};

#endif