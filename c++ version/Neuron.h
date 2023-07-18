#ifndef NEURON_H
#define NEURON_H

class Neuron
{
public:
    
    double activity;
    double pastActivity;

    Neuron(float activity) : activity(activity),pastActivity(activity) {}
    Neuron(){
        activity = 0;
        pastActivity = 0;
    }
};

#endif