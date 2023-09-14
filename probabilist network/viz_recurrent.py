#%%
import numpy as np
from matplotlib import pyplot as plt
import os
#%%
neuron_activities = dict()
neuron_coming_inh_syn = dict()
stim_node_activity = dict()
#%%
root_directory = '.'

# Loop through all subdirectories in the root directory
for dirpath, dirnames, filenames in os.walk(root_directory):
    for dirname in dirnames:
        if dirname.startswith("neuron_"):
            print("loading "+dirname)
            neuron_folder = os.path.join(dirpath, dirname)
            coming_inh_syn_folder = os.path.join(neuron_folder,"coming_inh_syn")

            activity_file_path = os.path.join(neuron_folder, "activity.txt")
            coming_inh_syn_file_path = os.path.join(coming_inh_syn_folder, "weight_evolution.txt")

            neuron_activities[dirname] = np.loadtxt(activity_file_path)
            neuron_coming_inh_syn[dirname] = np.loadtxt(coming_inh_syn_file_path)

# %%
window = 100
nb_ones = 0
nb_zeros = 0
cpt = 0

for key in neuron_activities:
    nb_ones_list = []

    for t in range(1,len(neuron_activities[key])):

        if (neuron_activities[key][t] == 1):
            nb_ones+=1

        if cpt==window:
            nb_ones_list.append(nb_ones)
            cpt = 0
            nb_ones=0

        cpt+=1
    plt.plot(nb_ones_list)

plt.show()


# %%
