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

    def __init__(self, source, target, w):
        # as the network is symmetric there is no source nor target
        self.source = source
        self.target = target
        self.w = w

class neuron :

    def __init__(self, state):

        self.state = state
        self.exc_syns = []
        self.inh_syns = []
        self.exc_w_sum = 0
        self.inh_w_sum = 0
    
    def add_exc_syn(self,syns):

        self.exc_syns = syns
        self.exc_w_sum = self.sum_weights_exc()
    
    def add_inh_syn(self,syns):

        self.inh_syns = syns
        self.inh_w_sum = self.sum_weights_inh()

    def sum_weights_exc(self):

        sum = 0
        for s in self.exc_syns:
            sum += s.w

        return sum
    
    def sum_weights_inh(self):

        sum = 0
        for s in self.inh_syns:
            sum += s.w

        return sum

    def linear_normalization_exc(self):

        new_sum = self.sum_weights_exc()
        diff = self.exc_w_sum-new_sum

        for i in range(len(self.exc_syns)):
            self.exc_syns[i].w = np.clip(self.exc_syns[i].w+diff/len(self.exc_syns),0,1)
        
        new_new_sum = self.sum_weights_exc()

        self.exc_w_sum = new_new_sum # Should stay the same as long we don't hit bound
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
            self.left_neuron.state = 0
            self.right_neuron.state = 1
        else:
            self.left_neuron.state = 1 
            self.right_neuron.state = 0 
    
    def decide_state(self):

        sum_drive_left = 0
        for i in range(len(self.left_neuron.exc_syns)):
            sum_drive_left+=self.left_neuron.exc_syns[i].source.state*self.left_neuron.exc_syns[i].w

        sum_drive_right = 0
        for i in range(len(self.right_neuron.exc_syns)):
            sum_drive_right+=self.right_neuron.exc_syns[i].source.state*self.right_neuron.exc_syns[i].w
        
        if sum_drive_left>sum_drive_right:
            self.change_state(1)
        else:
            self.change_state(0)
    
def pot_state_exc(unit_list,pot):

    for i in range(len(unit_list)):
        if unit_list[i].left_neuron.state == 1 :
            for k in range(len(unit_list[i].left_neuron.exc_syns)) :
                if unit_list[i].left_neuron.exc_syns[k].source.state == 1:
                    unit_list[i].left_neuron.exc_syns[k].w += pot
        else: # means unit_list[i].right_neuron == 1
            for k in range(len(unit_list[i].right_neuron.exc_syns)) :
                if unit_list[i].right_neuron.exc_syns[k].source.state == 1:
                    unit_list[i].right_neuron.exc_syns[k].w += pot

        unit_list[i].left_neuron.linear_normalization_exc()
        unit_list[i].right_neuron.linear_normalization_exc()


def print_state(unit_list,nb_row):
    print("unit states")
    cpt=0
    for i in range(len(unit_list)):
        if cpt < nb_row-1:
            print(unit_list[i].left_neuron.state,end="")
            cpt+=1
        else:
            print(unit_list[i].left_neuron.state)
            cpt=0

def change_all_states(unit_list,state):

    for i in range(len(unit_list)):
        if state[i] == 1 :
            unit_list[i].change_state(1)
        else :
            unit_list[i].change_state(0)
    
def main():

    exc_pot = 0.01
    nb_units = 30
    unit_list = []
    nb_syn_to_one = (nb_units-1)*2
    # vizualisation state
    nb_row = 5

    for i in range(nb_units):
        unit_list.append(unit(1))

    for i in range(nb_units):

        ln_0 = unit_list[i].left_neuron
        rn_0 = unit_list[i].right_neuron
        exc_syns_left = []
        inh_syns_left = []
        exc_syns_right = []
        inh_syns_right = []
            
        for j in range(nb_units):
            if i != j:
                ln_1 = unit_list[j].left_neuron
                rn_1 = unit_list[j].right_neuron
                exc_syns_left.append(syn(ln_1,ln_0, 1/nb_syn_to_one))
                exc_syns_left.append(syn(rn_1,ln_0, 1/nb_syn_to_one))
                inh_syns_left.append(syn(ln_1,ln_0, 1/nb_syn_to_one))
                inh_syns_left.append(syn(rn_1,ln_0, 1/nb_syn_to_one))

                exc_syns_right.append(syn(ln_1,rn_0, 1/nb_syn_to_one))
                exc_syns_right.append(syn(rn_1,rn_0, 1/nb_syn_to_one))
                inh_syns_right.append(syn(ln_1,rn_0, 1/nb_syn_to_one))
                inh_syns_right.append(syn(rn_1,rn_0, 1/nb_syn_to_one))

        ln_0.add_exc_syn(exc_syns_left)
        ln_0.add_inh_syn(inh_syns_left)

        rn_0.add_exc_syn(exc_syns_right)
        rn_0.add_inh_syn(inh_syns_right)
    

    # writing one attractor state :
    state = np.random.randint(0,2,nb_units)
    for i in range(len(unit_list)):
        if state[i] == 1 :
            unit_list[i].change_state(1)
        else :
            unit_list[i].change_state(0)

    print_state(unit_list,nb_row)

    pot_state_exc(unit_list,exc_pot)


    # Random begining state
    state = np.random.randint(0,2,nb_units)
    change_all_states(unit_list,state)
    print_state(unit_list,nb_row)

    for u in unit_list:
        u.decide_state() 
    
    print_state(unit_list,nb_row)

    


if __name__=="__main__":
    main()

# MISC CODE can be reused

# print("synapse matrix")
# for i in range(len(unit_list)):

#     print("unit: "+str(i))
#     print("left neuron synapses")
#     for j in range(len(unit_list[i].left_neuron.exc_syns)):
#         print(unit_list[i].left_neuron.exc_syns[j].w,end="")
#     print("")
#     print("right neuron synapses")
#     for j in range(len(unit_list[i].right_neuron.exc_syns)):
#         print(unit_list[i].right_neuron.exc_syns[j].w,end="")
#     print("")

# choosed_unit = 0
# cpt=0
# for i in range(len(unit_list[i].left_neuron.exc_syns)):
#     if cpt<row-1:
#         print(str(unit_list[0].left_neuron.exc_syns[i].w)[:4],end="  ")
#         cpt+=1
#     else:
#         print(str(unit_list[0].left_neuron.exc_syns[i].w)[:4])
#         cpt=0
# print("")