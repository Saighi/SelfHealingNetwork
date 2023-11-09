# Hopfield like network.
# Each cell has two possible state in a winner takes all manner as binary hopfield models.
# Asynchronous random update.
# Random initialization.
# Symmetric weights.
# Excitatory Weights as wij belongs to N between 0 and 1.
# TOTAL Inhibitory weights have to be largely stronger than 
# Excitatory weights with normalization, either linear or multiplicative.
# Inhibitory weights with additive normalization and supar linear increase will be bounded by excitatory weights.
#%%
from random import randint
#%%
class link:

    def __init__(self, w, source, target):
        
        self.w_plus = w
        self.w_minus = w
        self.source = source
        self.target = target

class neuron :
    def __init__(self, state):
        self.state = state
        self.exc_syns = []
        self.inh_syns = []

class unit:

    def __init__(self):

        rd_state = randint(0,1)

        if rd_state == 0:
            self.left_neuron = neuron(0) 
            self.right_neuron = neuron(1) 
        else:
            self.left_neuron = neuron(1) 
            self.right_neuron = neuron(0) 


    def __init__(self, state):

        self.state = state 
        self.exc_syns = []
        self.inh_syns = []
    
    def __init__(self, state, exc_syns, inh_syns):

        self.state = state 
        self.exc_syns = exc_syns
        self.inh_syns = inh_syns
    
    def change_state(self):
        
        inputs = [0,0]

        for syn in self.exc_syns:


        