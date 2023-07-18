import numpy as np

def global_connectivity(nb_neurons,probability):
    all_neurons_id = np.arange(0,nb_neurons)
    all_i = np.repeat(all_neurons_id,nb_neurons)
    all_j = np.tile(all_neurons_id,nb_neurons)
    chozen_connections = np.random.choice(np.arange(0,nb_neurons**2),int((nb_neurons**2)*probability),replace=False)
    i_connect = all_i[chozen_connections]
    j_connect = all_j[chozen_connections]
    return i_connect,j_connect

def strength_assembly(neuron_pool,i_connect,j_connect,prev_strengths,inside_strength):

    new_strengths = prev_strengths

    for i_id in range(len(i_connect)):
        i= i_connect[i_id]
        j=j_connect[i_id]
        if i in neuron_pool and j in neuron_pool:
            new_strengths[i_id] = inside_strength

    return new_strengths