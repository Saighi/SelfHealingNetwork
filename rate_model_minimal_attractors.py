import numpy as np
import matplotlib.pyplot as plt
import math


pool_size = 2
number_units = 2
max_activity = 1

first_unit = np.full(pool_size,0.25)
first_unit_past = np.copy(first_unit)
recurent_strength = 0.05
inhib_strength = 0.03
number_iteration = 1000
inhibitory_decay = 0.0005
inhibitory_pot = 0.005
inhibitory_unit = np.zeros(pool_size)
inhibitory_unit_past=np.zeros(pool_size)
inhibitory_weights = np.full(pool_size,0)
history = []

number_peak = np.zeros(pool_size)
previous = 0

for t in range(number_iteration):

    history.append(np.copy(first_unit))

    for i in range(pool_size):

        if first_unit[i]>0.5 and previous !=i:
            number_peak[i]+=1
            previous=i

        first_unit[i] = min(max_activity, first_unit[i]+(first_unit_past[i]*recurent_strength))
        inhibitory_unit[i] = first_unit_past[i]*inhibitory_weights[i]
        first_unit[i] = max(0, first_unit[i] - inhibitory_unit_past[i])

        if first_unit[i]>0.5 :
            inhibitory_weights[i] = inhibitory_weights[i]+inhibitory_pot

        for j in range(pool_size):

            if i!=j :
                first_unit[j] = max(0,first_unit[j] - (first_unit_past[i]*inhib_strength))


    inhibitory_weights = np.clip(inhibitory_weights-inhibitory_decay,0,100)

    first_unit = np.clip(first_unit+np.random.uniform(-0.02,0.02,pool_size),0,max_activity)

    first_unit_past=np.copy(first_unit)
    inhibitory_unit_past = np.copy(inhibitory_unit)

print(number_peak)
# history = np.array(history)
# for i in range(pool_size):
#     plt.plot(history[:,i])

# plt.show()
