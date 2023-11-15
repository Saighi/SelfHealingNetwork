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
# %%
import matplotlib.pyplot as plt
import numpy as np


# %%

class Syn:

    def __init__(self, source, target, w):
        # as the network is symmetric there is no source nor target
        self.source = source
        self.target = target
        self.w = w


class Neuron:

    def __init__(self, state):

        self.state = state
        self.exc_syns = []
        self.inh_syns = []
        self.exc_w_sum = 0
        self.inh_w_sum = 0

    def add_exc_syn(self, syns):

        self.exc_syns = syns
        self.exc_w_sum = self.sum_weights_exc()

    def add_inh_syn(self, syns):

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
        diff = self.exc_w_sum - new_sum

        for i in range(len(self.exc_syns)):
            self.exc_syns[i].w = np.clip(self.exc_syns[i].w + diff / len(self.exc_syns), 0, 1)

        new_new_sum = self.sum_weights_exc()

        self.exc_w_sum = new_new_sum  # Should stay the same as long we don't hit bound
        # Should then be REMOVED when plasticity rules are stable
    
    def mult_normalization_exc(self):

        new_sum = self.sum_weights_exc()

        for i in range(len(self.exc_syns)):
            self.exc_syns[i].w = self.exc_syns[i].w/new_sum

        self.exc_w_sum = self.sum_weights_exc() # Should stay the same as long we don't hit bound
        # Should then be REMOVED when plasticity rules are stable

    def linear_normalization_inh(self):

        new_sum = self.sum_weights_inh()
        diff = self.inh_w_sum - new_sum

        for i in range(len(self.inh_syns)):
            self.inh_syns[i].w = np.clip(self.inh_syns[i].w + diff / len(self.inh_syns), 0, 1)

        new_new_sum = self.sum_weights_inh()

        self.inh_w_sum = new_new_sum  # Should stay the same as long we don't hit bound
        # Should then be REMOVED when plasticity rules are stable


class Unit:

    def __init__(self, state):

        if state == 0:
            self.left_neuron = Neuron(0)
            self.right_neuron = Neuron(1)
        else:
            self.left_neuron = Neuron(1)
            self.right_neuron = Neuron(0)

    def change_state(self, state):

        if state == 0:
            self.left_neuron.state = 0
            self.right_neuron.state = 1
        else:
            self.left_neuron.state = 1
            self.right_neuron.state = 0

    def decide_state(self):

        sum_drive_left = 0
        for i in range(len(self.left_neuron.exc_syns)):
            sum_drive_left += self.left_neuron.exc_syns[i].source.state * self.left_neuron.exc_syns[i].w

        sum_drive_right = 0
        for i in range(len(self.right_neuron.exc_syns)):
            sum_drive_right += self.right_neuron.exc_syns[i].source.state * self.right_neuron.exc_syns[i].w

        sum_inh_left = 0
        for i in range(len(self.left_neuron.inh_syns)):
            sum_inh_left += self.left_neuron.inh_syns[i].source.state * self.left_neuron.inh_syns[i].w

        sum_inh_right = 0
        for i in range(len(self.right_neuron.inh_syns)):
            sum_inh_right += self.right_neuron.inh_syns[i].source.state * self.right_neuron.inh_syns[i].w

        if (sum_drive_left - sum_inh_left) > (sum_drive_right - sum_inh_right):
            self.change_state(1)
        else:
            self.change_state(0)


def pot_state_exc(unit_list, pot):
    for i in range(len(unit_list)):
        if unit_list[i].left_neuron.state == 1:
            for k in range(len(unit_list[i].left_neuron.exc_syns)):
                if unit_list[i].left_neuron.exc_syns[k].source.state == 1:
                    unit_list[i].left_neuron.exc_syns[k].w += pot
        else:  # means unit_list[i].right_neuron == 1
            for k in range(len(unit_list[i].right_neuron.exc_syns)):
                if unit_list[i].right_neuron.exc_syns[k].source.state == 1:
                    unit_list[i].right_neuron.exc_syns[k].w += pot

        unit_list[i].left_neuron.linear_normalization_exc()
        unit_list[i].right_neuron.linear_normalization_exc()

def pot_state_exc_mult(unit_list, pot):
    for i in range(len(unit_list)):
        if unit_list[i].left_neuron.state == 1:
            for k in range(len(unit_list[i].left_neuron.exc_syns)):
                if unit_list[i].left_neuron.exc_syns[k].source.state == 1:
                    unit_list[i].left_neuron.exc_syns[k].w += pot
        else:  # means unit_list[i].right_neuron == 1
            for k in range(len(unit_list[i].right_neuron.exc_syns)):
                if unit_list[i].right_neuron.exc_syns[k].source.state == 1:
                    unit_list[i].right_neuron.exc_syns[k].w += pot

        unit_list[i].left_neuron.mult_normalization_exc()
        unit_list[i].right_neuron.mult_normalization_exc()

def pot_state_inh(unit_list, pot):
    for i in range(len(unit_list)):
        if unit_list[i].left_neuron.state == 1:

            for k in range(len(unit_list[i].left_neuron.inh_syns)):
                if unit_list[i].left_neuron.inh_syns[k].source.state == 1:
                    unit_list[i].left_neuron.inh_syns[k].w += pot

        else:  # means unit_list[i].right_neuron == 1

            for k in range(len(unit_list[i].right_neuron.inh_syns)):
                if unit_list[i].right_neuron.inh_syns[k].source.state == 1:
                    unit_list[i].right_neuron.inh_syns[k].w += pot

        unit_list[i].left_neuron.linear_normalization_inh()
        unit_list[i].right_neuron.linear_normalization_inh()


def print_state(unit_list, nb_row):
    print("unit states")
    cpt = 0
    for i in range(len(unit_list)):
        if cpt < nb_row - 1:
            print(unit_list[i].left_neuron.state, end="")
            cpt += 1
        else:
            print(unit_list[i].left_neuron.state)
            cpt = 0


def change_all_states(unit_list, state):
    for i in range(len(unit_list)):
        if state[i] == 1:
            unit_list[i].change_state(1)
        else:
            unit_list[i].change_state(0)


def get_state(unit_list):
    states_list = []
    for i in range(len(unit_list)):
        states_list.append(unit_list[i].left_neuron.state)
    return np.array(states_list)


def exc_syn_dist(unit_list):
    syn_weights_dist = []
    for i in range(len(unit_list)):
        for j in range(len(unit_list[i].left_neuron.exc_syns)):
            syn_weights_dist.append(unit_list[i].left_neuron.exc_syns[j].w)
        for j in range(len(unit_list[i].right_neuron.exc_syns)):
            syn_weights_dist.append(unit_list[i].right_neuron.exc_syns[j].w)

    return syn_weights_dist


def inh_syn_dist(unit_list):
    syn_weights_dist = []
    for i in range(len(unit_list)):
        for j in range(len(unit_list[i].left_neuron.inh_syns)):
            syn_weights_dist.append(unit_list[i].left_neuron.inh_syns[j].w)
        for j in range(len(unit_list[i].right_neuron.inh_syns)):
            syn_weights_dist.append(unit_list[i].right_neuron.inh_syns[j].w)

    return syn_weights_dist


def inner_state_inh_w(unit_list):
    sum_in_w = 0
    for i in range(len(unit_list)):
        if unit_list[i].left_neuron.state == 1:
            for k in range(len(unit_list[i].left_neuron.inh_syns)):
                if unit_list[i].left_neuron.inh_syns[k].source.state == 1:
                    sum_in_w += unit_list[i].left_neuron.inh_syns[k].w
        else:  # means unit_list[i].right_neuron == 1
            for k in range(len(unit_list[i].right_neuron.inh_syns)):
                if unit_list[i].right_neuron.inh_syns[k].source.state == 1:
                    sum_in_w += unit_list[i].right_neuron.inh_syns[k].w
    return sum_in_w


def making_network(nb_units):
    nb_syn_to_one = (nb_units - 1) * 2
    unit_list = []
    for i in range(nb_units):
        unit_list.append(Unit(1))

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
                exc_syns_left.append(Syn(ln_1, ln_0, 1 / nb_syn_to_one))
                exc_syns_left.append(Syn(rn_1, ln_0, 1 / nb_syn_to_one))
                inh_syns_left.append(Syn(ln_1, ln_0, 1 / nb_syn_to_one))
                inh_syns_left.append(Syn(rn_1, ln_0, 1 / nb_syn_to_one))

                exc_syns_right.append(Syn(ln_1, rn_0, 1 / nb_syn_to_one))
                exc_syns_right.append(Syn(rn_1, rn_0, 1 / nb_syn_to_one))
                inh_syns_right.append(Syn(ln_1, rn_0, 1 / nb_syn_to_one))
                inh_syns_right.append(Syn(rn_1, rn_0, 1 / nb_syn_to_one))

        ln_0.add_exc_syn(exc_syns_left)
        ln_0.add_inh_syn(inh_syns_left)

        rn_0.add_exc_syn(exc_syns_right)
        rn_0.add_inh_syn(inh_syns_right)

    return unit_list


def main():

    inh_pot = 0.0001
    # inh_pot = 0.00125 add
    exc_write = 0.025 

    nb_iter_writing = 100

    nb_units = 50
    unit_list = making_network(nb_units)

    # vizualisation state
    nb_row = 5

    nb_writed_states = 4 
    writed_state_list = []

    for i in range(nb_writed_states):

        writed_state = np.random.randint(0, 2, nb_units)
        writed_state_list.append(writed_state)
        print(writed_state)

    for j in range(nb_iter_writing):
        for state in writed_state_list:

            change_all_states(unit_list, state)
            #pot_state_exc(unit_list, exc_write/nb_iter_writing)
            pot_state_exc_mult(unit_list, exc_write/nb_iter_writing)
            for u in unit_list:
                u.decide_state()


    # plt.hist(exc_syn_dist(unit_list))
    # plt.show()

    print("exploring patterns with inhibition :")
    meta_state_list = []
    nb_iter= 250
    distance_to_writed=np.zeros((nb_writed_states,nb_iter))

    for i in range(nb_iter):

        for j in range(nb_writed_states):
            writed_state = writed_state_list[j]
            dist_to_inverse = np.count_nonzero(get_state(unit_list) != np.where(writed_state == 1, 0, 1))
            dist = np.count_nonzero(get_state(unit_list) != writed_state)
            distance_to_writed[j,i]=min(dist_to_inverse, dist)

        state = np.random.randint(0, 2, nb_units)
        change_all_states(unit_list, state)
        for u in unit_list:
            u.decide_state()
        #print_state(unit_list,nb_row)

        meta_state_list.append(get_state(unit_list))

        pot_state_inh(unit_list, inh_pot)
    
    plt.plot(distance_to_writed.T)
    plt.show()





if __name__ == "__main__":
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

