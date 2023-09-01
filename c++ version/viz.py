
#%%
import numpy as np
from matplotlib import pyplot as plt
import os
#%%

neuron_activities = dict()
neuron_coming_exc_syn = dict()
neuron_coming_lateral_inh_syn = dict()
#%%

# Specify the root directory where you want to start searching
root_directory = '.'

# Loop through all subdirectories in the root directory
for dirpath, dirnames, filenames in os.walk(root_directory):
    for dirname in dirnames:
        if dirname.startswith("neuron_"):
            print("loading "+dirname)
            neuron_folder = os.path.join(dirpath, dirname)
            coming_exc_syn_folder = os.path.join(neuron_folder,"coming_exc_syn")
            coming_lateral_inh_syn_folder = os.path.join(neuron_folder,"coming_lateral_inh_syn")

            activity_file_path = os.path.join(neuron_folder, "activity.txt")
            coming_exc_syn_file_path = os.path.join(coming_exc_syn_folder, "activity.txt")
            coming_lateral_inh_syn_file_path = os.path.join(coming_lateral_inh_syn_folder, "activity.txt")

            neuron_activities[dirname] = np.loadtxt(activity_file_path)
            # neuron_coming_exc_syn[dirname] = np.loadtxt(coming_exc_syn_file_path)
            # neuron_coming_lateral_inh_syn[dirname] = np.loadtxt(coming_lateral_inh_syn_file_path)

            # for d in data:
            #     print(d)

            # # Check if the "activity" file exists in the current "neuron_" folder
            # if os.path.exists(activity_file_path):
            #     # Read the contents of the "activity" file
            #     with open(activity_file_path, 'r') as activity_file:
            #         activity_data = activity_file.read()
            #         # Process the content of the "activity" file as needed
            #         print(f"Activity data in {activity_file_path}:\n{activity_data}")
#%%
for key in neuron_activities:
    print(key)
    plt.plot(neuron_activities[key])
plt.show()
# # %%
# plt.plot(neuron_coming_lateral_inh_syn["neuron_8"].T)
# plt.show()
# # %%
# plt.plot(neuron_coming_exc_syn["neuron_8"].T)
# plt.show()
# # %%
# print(neuron_activities["neuron_1"])
# # %%
# for key in neuron_activities:
#     plt.plot(neuron_activities[key])
# plt.show()
# #%%

# %%
