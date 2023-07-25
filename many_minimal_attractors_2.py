import numpy as np
import matplotlib.pyplot as plt
from numpy.lib.stride_tricks import as_strided

number_units = 2
pool_size = 2
max_activity = 1

units = np.full((number_units,pool_size),0.25)
units_past = np.copy(units)

recurent_strength = 0.05
lateral_inhib_strength = 0.05
number_iteration = 500
inhibitory_decay = 0.001
inhibitory_pot = 0.005
inhibitory_units = np.zeros((number_units,pool_size))
inhibitory_units_past= np.copy(inhibitory_units)
all_inhibitory_weights = np.full((number_units,pool_size),0.)

inter_units_exc_synapses = np.full((number_units*pool_size,number_units*pool_size), 0.)
inter_units_exc_to_inh_synapses = np.full((number_units*pool_size,number_units*pool_size), 0.)

neurons_targets_activity_past = dict()
neurons_targets_activity = dict()
neurons_outward_synapses = dict()


for j in range(number_units):
    for i in range(pool_size):
        targets_x = []
        targets_y = []
        synapses_x = []
        synapses_y = []
        for m in range(number_units):
            for n in range(pool_size):
                if m != j:
                    targets_x.append(m)
                    targets_y.append(n)
                    synapses_x.append((j*pool_size)+i)
                    synapses_y.append((m*pool_size)+n)
                    if i == 0 and n == 0:
                        inter_units_exc_synapses[(j*pool_size)+i,(m*pool_size)+n] = 0.05
                    if i == 1 and n == 1:
                        inter_units_exc_synapses[(j * pool_size) + i, (m * pool_size) + n] = 0.05

        print(targets_x)
        print(targets_y)
        print(j,i)
        neurons_targets_activity_past[(j, i)] = units_past[targets_x,targets_y]
        neurons_targets_activity[(j, i)] = units[np.ix_([targets_x,targets_y])]
        neurons_outward_synapses[(j, i)] = inter_units_exc_synapses[synapses_x,synapses_y]


# for j in range(number_units):
#     for i in range(pool_size):
#         for m in range(number_units):
#             for n in range(pool_size):
#                 if i == 0 and n == 0:
#                     inter_units_exc_to_inh_synapses[(j*pool_size)+i,(m*pool_size)+n] = 0.05
#                 if i == 1 and n == 1:
#                     inter_units_exc_to_inh_synapses[(j * pool_size) + i, (m * pool_size) + n] = 0.05

history = []

number_peak = np.zeros((number_units, pool_size))
previous = 0
neuron_index = 0

for t in range(number_iteration):

    neuron_index = 0
    history.append(np.copy(units))

    for j in range(number_units):

        for i in range(pool_size):

            if units[j, i]>0.5 and previous!=i:
                number_peak[j, i]+=1
                previous = i

            units[j, i] = min(max_activity, units[j, i] + (units_past[j, i] * recurent_strength))
            inhibitory_units[j, i] = units_past[j, i] * all_inhibitory_weights[j, i]
            units[j, i] = max(0, units[j, i] - inhibitory_units_past[j, i])

            if units[j, i]>0.5 :
                all_inhibitory_weights[j, i] += inhibitory_pot
                #inter_units_exc_to_inh_synapses[(j*pool_size)+i,:]+=inhibitory_pot*neurons_targets[(i, j)]

            for k in range(pool_size):

                if i!=k :
                    units[j,k] = max(0, units[j,k] - (units_past[j, i] * lateral_inhib_strength))

            #neurons_targets_activity[(j, i)] = np.clip(neurons_targets_activity[(j, i)]+units_past[j, i]* neurons_outward_synapses[(j, i)], 0, 1)
            print(neurons_targets_activity)
            # for m in range(number_units):
            #     for n in range(pool_size):
            #         units[m,n] = min(units[m,n]+units_past[j,i]*inter_units_exc_synapses[(j*pool_size)+i,(m*pool_size)+n], 1)

            # for m in range(number_units):
            #     for n in range(pool_size):
            #         inhibitory_units[m,n] = min(inhibitory_units[m,n]+inhibitory_units_past[j,i]*inter_units_exc_to_inh_synapses[(j*pool_size)+i,(m*pool_size)+n], 1)

            neuron_index+=1

        all_inhibitory_weights[j]=np.clip(all_inhibitory_weights[j]-inhibitory_decay, 0, 100)
        units[j]=np.clip(units[j] + np.random.uniform(-0.02, 0.02, pool_size), 0, max_activity)
        units_past[j]=np.copy(units[j])
        inhibitory_units_past[j] = np.copy(inhibitory_units[j])

fig, axs = plt.subplots(2)
fig.suptitle('Vertically stacked subplots')

history = np.array(history)
for j in range(number_units):
    for i in range(pool_size):
        axs[j].plot(history[:,j, i])

plt.show()

