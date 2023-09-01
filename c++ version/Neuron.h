#ifndef NEURON_H
#define NEURON_H

class Neuron
{
private :

    static int nextId;
    int id;

public:
    
    double activity;
    double pastActivity;

    Neuron(float activity) : activity(activity),pastActivity(activity) {id = nextId++;}
    Neuron(){
        id = nextId++;
        activity = 0;
        pastActivity = 0;
    }

    int getID() const {
        return id;
    }
};

#endif