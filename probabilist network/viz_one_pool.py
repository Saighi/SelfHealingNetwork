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
    stim_node_activity["node_1"] = np.loadtxt("activity_stim_node.txt")   
    stim_node_activity["node_2"] = np.loadtxt("activity_stim_node_2.txt")   
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
plt.plot(neuron_coming_inh_syn["neuron_1"].T)
# %%
window = 5000
nb_ones = 0
nb_zeros = 0
cpt = 0
ratios = []

for t in range(1,len(stim_node_activity["node_1"])):

    if stim_node_activity["node_1"][t-1] == 0:
        if neuron_activities["neuron_1"][t] == 0:
            nb_ones+=1
        else:
            nb_zeros+=1

    if cpt==window:
        ratios.append(nb_ones/nb_zeros)
        cpt = 0
        nb_ones=0
        nb_zeros=0

    cpt+=1



# %%
plt.plot(ratios)
# %%
