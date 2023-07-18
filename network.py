import numpy as np
from brian2 import *
import random

time_sim = 100*second

nb_inputs = 1000
nb_neurons = 100
nb_connection_input_neuron = 10

w_input = 0.4
w_inh_inputs = 0.3
tau = 10*ms
tau_h = 3000*ms
tau_heb_inh = 200*ms

learning_rate_heb_inh = 1/50
depression_inh_heb = 1/100
#learning_rate_heb_inh = 0
#depression_inh_heb = 0
learning_rate_stdp = 1/1500

h_equilibrium = 50
homeostatic_rate = 1/10000

"""Neurons"""
#inputs
stimulus_array = np.full(int((time_sim/second)/0.1),10)
stimulus = TimedArray(stimulus_array*Hz, dt=100.*ms)
network_inputs = PoissonGroup(nb_inputs, rates='stimulus(t)')

#excitatory neurons
eqs = '''
dv/dt = -v/tau : 1
dh/dt = -h/tau_h : 1
'''
neurons = NeuronGroup(nb_neurons, eqs, threshold='v>1', reset='v=0', method='euler',refractory=10*ms)

"""Synapses"""

stim_neurons_exc_syn = Synapses(network_inputs,neurons,on_pre="v+=w_input")
i_connect_stim = np.arange(0,nb_inputs)
j_connect_stim = np.repeat(np.arange(0, nb_neurons),nb_connection_input_neuron)
stim_neurons_exc_syn.connect(i=i_connect_stim, j=j_connect_stim)

model_stim_syn = '''
w : 1
dwindow/dt = -window/tau_heb_inh : 1
'''

on_pre_stim_inh = '''
v-=w
window+=1
w = clip(w-depression_inh_heb,0,1000)
'''
on_post_stim_inh = '''
w = clip(w+(window*learning_rate_heb_inh), 0, 1000) 
'''

stim_neurons_inh_syn = Synapses(network_inputs,neurons,model=model_stim_syn,on_pre=on_pre_stim_inh,on_post=on_post_stim_inh,delay=1*ms)
i_connect_stim_inh = np.random.choice(np.arange(0, nb_inputs),nb_inputs,replace=False)
j_connect_stim_inh = np.repeat(np.arange(0, nb_neurons), nb_connection_input_neuron)
stim_neurons_inh_syn.connect(i=i_connect_stim_inh, j=j_connect_stim_inh)
stim_neurons_inh_syn.w = w_input

model_stdp_syn = '''
w : 1
dwindow_pot/dt = -window_pot/tau_heb_inh : 1
dwindow_dep/dt = -window_dep/tau_heb_inh : 1
'''

on_pre_stdp_syn = '''
v+=w
h+=w
window_pot += 1
w=clip(w-(window_dep*learning_rate_stdp)+(h_equilibrium-h_post)*homeostatic_rate,0,0.25)
'''

on_post_stdp_syn = '''
window_dep += 1
w = clip(w+(window_pot*learning_rate_stdp),0,0.25)
'''

model_heb_syn = '''
w : 1
dwindow/dt = -window/tau_heb_inh : 1
'''

on_pre_heb_inh = '''
v-=w
window+=1
w = clip(w-depression_inh_heb,0,1000)
'''
on_post_heb_inh = '''
w = clip(w+(window*learning_rate_heb_inh), 0, 1000) 
'''

stdp_recurrent_exc_syn = Synapses(neurons,neurons,model=model_stdp_syn,on_pre=on_pre_stdp_syn, on_post=on_post_stdp_syn)
i_connect_stdp = np.tile(np.arange(0,nb_neurons),nb_neurons)
j_connect_stdp = np.repeat(np.arange(0,nb_neurons),nb_neurons)
stdp_recurrent_exc_syn.connect(i=i_connect_stdp,j=j_connect_stdp)
stdp_recurrent_exc_syn.w = np.random.exponential((w_input/(nb_neurons/nb_inputs))/40,nb_neurons*nb_neurons)

recurrent_inh_syn = Synapses(neurons,neurons,model=model_stdp_syn,on_pre="v-=w",delay=1*ms)
recurrent_inh_syn.connect()
#recurrent_inh_syn.w = np.random.exponential((w_input/(nb_neurons/nb_inputs))/200,nb_neurons*nb_neurons)

recurrent_hebbian_inh_syn = Synapses(neurons, neurons, model = model_heb_syn, on_pre=on_pre_heb_inh, on_post=on_post_heb_inh,delay=1*ms)
recurrent_hebbian_inh_syn.connect()
recurrent_hebbian_inh_syn.w = np.random.exponential((w_input/(nb_neurons/nb_inputs))/100,nb_neurons*nb_neurons)
#recurrent_hebbian_inh_syn.w = 0

"""Monitors"""
neurons_mon = StateMonitor(neurons, "v", record=[0])
neurons_mon_h = StateMonitor(neurons, "h", record=[0])
spike_mon = SpikeMonitor(neurons)
heb_inh_mon = StateMonitor(recurrent_hebbian_inh_syn, "w", record=np.arange(nb_neurons*nb_neurons),dt=0.1*second)
stim_inh_mon = StateMonitor(stim_neurons_inh_syn, "w", record=np.arange(nb_neurons*nb_connection_input_neuron),dt=0.1*second)
stdp_exc_mon = StateMonitor(stdp_recurrent_exc_syn, "w", record=np.arange(nb_neurons*nb_neurons),dt=0.1*second)
exc_rate_mon = PopulationRateMonitor(neurons)

run(time_sim, report='text', report_period=5 * second)

print(neurons_mon.v[0])
plt.plot(neurons_mon.v[0])
plt.show()

print(neurons_mon_h.h[0])
plt.plot(neurons_mon_h.h[0])
plt.show()

# spikes_first_neuron = []
# for i in range(len(spike_mon.t)):
#     if spike_mon.i[i] == 0 :
#         spikes_first_neuron.append(spike_mon.t[i])
#
# print(spikes_first_neuron)
# plt.eventplot(spikes_first_neuron)
plt.scatter(spike_mon.t,spike_mon.i,s=1)

plt.show()
for i in range(100):
    plt.plot(heb_inh_mon.w[i])

plt.show()

for i in range(100):
    plt.plot(stim_inh_mon.w[i])

plt.show()

for i in range(100):
    plt.plot(stdp_exc_mon.w[i])

plt.show()

plt.hist(stdp_recurrent_exc_syn.w)
plt.show()

i=0
while i<10:
    plt.hist(stdp_recurrent_exc_syn.w[nb_neurons*i:(i+1)*nb_neurons])
    plt.show()
    i+=1

plt.plot(exc_rate_mon.smooth_rate(window='gaussian',width=100*ms))

plt.show()