# Hopfield like network.
# Each cell has two possible state in a winner takes all manner as binary hopfield models.
# Asynchronous random update.
# Random initialization.
# Symmetric weights.
# Excitatory Weights as wij belongs to N between 0 and 1.
# TOTAL Inhibitory weights have to be largely stronger than 
# Excitatory weights with normalization, either linear or multiplicative.
# Inhibitory weights with additive normalization and supar linear increase will be bounded by excitatory weights.
# First step no excitatory plasticity
#%%
from random import randint
import numpy as np
#%%

class syn:

    def __init__(self, a, b):
        # as the network is symmetric there is no source nor target
        self.source = a
        self.target = b

class neuron :

    def __init__(self, state):

        self.state = state
        self.exc_syns = []
        self.exc_weights = np.array([])
        self.inh_syns = []
        self.inh_weights = np.array([])
        self.exc_w_sum = 0
        self.inh_w_sum = 0
    
    def add_exc_syn(self,syns,weights):

        self.exc_weights = weights
        self.exc_syns = syns
        self.exc_w_sum = np.sum(weights)
    
    def add_inh_syn(self,syns,weights):

        self.inh_weights = weights
        self.inh_syns = syns
        self.inh_w_sum = np.sum(weights)
    
    def linear_normalization_exc(self):
        new_sum = np.sum(self.exc_weights)
        diff = self.sum-new_sum
        self.exc_weights+=diff/len(self.exc_weights)
        self.sum = np.sum(self.exc_weights) # Should stay the same as long we don't hit bound
                                            # Should then be REMOVED when plasticity rules are stable 

class unit:
    
    def __init__(self, state):

        if state == 0:
            self.left_neuron = neuron(0) 
            self.right_neuron = neuron(1) 
        else:
            self.left_neuron = neuron(1) 
            self.right_neuron = neuron(0) 

    
    def change_state(self,state):
        
        if state == 0:
            self.left_neuron = neuron(0) 
            self.right_neuron = neuron(1) 
        else:
            self.left_neuron = neuron(1) 
            self.right_neuron = neuron(0)
    
    def decide_state(self):

        left_drive = np.sum(self.left_neuron.exc_weights)-np.sum(self.left_neuron.inh_weights)
        right_drive = np.sum(self.right_neuron.exc_weights)-np.sum(self.right_neuron.inh_weights)
        
        if left_drive>right_drive:
            self.change_state(1)
        else:
            self.change_state(0)

def main():
    nb_units = 2


if __name__=="__main__":
    main()



        