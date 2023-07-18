from brian2 import *
from tools import *
import numpy as np
from collections import defaultdict
import neo
from elephant import statistics
import quantities as pq
from elephant import kernels

time_sim = 20*second
nb_neurons = 200
nb_inputs = nb_neurons*10
w_input = 0.35

tau = 10*ms
tau_inh = 10*ms
tau_exc_inh = 200*ms
tau_x = 200*ms
tau_self_exc = 400*ms
tau_inh_exc = 400*ms
v_self_exc = 4
v_exc = 4
v_inh = -1
w_recurrent = 0.05
stimulus_array = np.full(int((time_sim/second)/0.1),10)
stimulus = TimedArray(stimulus_array*Hz, dt=100.*ms)
network_inputs = PoissonGroup(nb_inputs, rates='stimulus(t)')


eqs = '''
dexc_inh/dt = -exc_inh/tau_exc_inh : Hz
dself_exc/dt = -self_exc/tau_self_exc : Hz
dv/dt = (-v/tau) + (self_exc*(v_self_exc-v))+(exc_inh*(v_inh-v)) : 1
'''

eqs_inh = '''
dinh_exc/dt = -inh_exc/tau_inh_exc : Hz
dv/dt = (-v/tau_inh) + (inh_exc*(v_exc-v)) : 1
'''

model_recurrent= '''
dx/dt = (1-x)/tau_x :1
w : 1
'''

on_pre_recurrent = '''
self_exc+=(x*w)*Hz
x = x-0.04
'''

neurons = NeuronGroup(nb_neurons, eqs, threshold='v>1', reset='v=0', method='euler',refractory=3*ms)
inh_neurons = NeuronGroup(int(nb_neurons/5), eqs_inh, threshold='v>0.7', reset='v=0', method='euler',refractory=0.3*ms)

stim_neurons_exc_syn = Synapses(network_inputs, neurons,on_pre="v+=w_input")
i_connect_stim = np.arange(0,nb_inputs)
j_connect_stim = np.repeat(np.arange(0, nb_neurons), 10)
stim_neurons_exc_syn.connect(i=i_connect_stim,j=j_connect_stim)

stim_neurons_inh_syn = Synapses(network_inputs, inh_neurons,on_pre="v+=w_input/5")
i_connect_stim_inh = np.arange(0,nb_inputs)
j_connect_stim_inh = np.repeat(np.arange(0, int(nb_neurons/5)), 50)
stim_neurons_inh_syn.connect(i= i_connect_stim, j = j_connect_stim_inh)

recurrent = Synapses(neurons,neurons,model=model_recurrent,on_pre=on_pre_recurrent)
i_connect_recurrent,j_connect_recurrent = global_connectivity(nb_neurons,0.05)
recurrent.connect(i=i_connect_recurrent,j = j_connect_recurrent)
recurrent.w = np.repeat(w_recurrent,len(i_connect_recurrent))
recurrent.connect(p=0.1)
recurrent.w = w_recurrent

""" Dealing with assemblies in excitatory recurence"""
assembly_1 = np.random.choice(np.arange(0, nb_neurons), 100)
new_rec_w = strength_assembly(assembly_1, i_connect_recurrent, j_connect_recurrent, recurrent.w, 0.35)
new_rec_w = np.array(list(new_rec_w))
#np.random.shuffle(new_rec_w)
recurrent.w = new_rec_w
print(list(recurrent.w))

"""End of dealing with assemblies"""

inh_to_exc = Synapses(inh_neurons, neurons,on_pre="exc_inh+=0.3*Hz")
inh_to_exc.connect(p=0.1)

exc_to_inh = Synapses(neurons, inh_neurons,on_pre="inh_exc+=0.15*Hz")
exc_to_inh.connect(p=0.1)

neurons_mon = StateMonitor(neurons, ["v", "self_exc"], record=[0])
spike_mon = SpikeMonitor(neurons)
inh_rate_mon = PopulationRateMonitor(inh_neurons)
exc_rate_mon = PopulationRateMonitor(neurons)

run(time_sim, report='text', report_period=5 * second)

plt.plot(neurons_mon.v[0])
plt.show()

# plt.plot(neurons_mon.self_exc[0])
# plt.show()

plt.plot(exc_rate_mon.smooth_rate(window='gaussian', width=50*ms))
plt.show()

plt.plot(inh_rate_mon.smooth_rate(window='gaussian', width=50*ms))
plt.show()

spiketrains = defaultdict(list)

for i in range(len(spike_mon.i)):
    index = spike_mon.i[i]
    time = spike_mon.t[i]
    spiketrains[index].append(time)

neo_spiketrains = dict()
rates = dict()
kernel = kernels.GaussianKernel(sigma=500 * pq.ms)

for key in spiketrains:
    neo_spiketrains[key] = neo.SpikeTrain(spiketrains[key], t_stop=time_sim, units='s')
    rate = statistics.instantaneous_rate(neo_spiketrains[key], sampling_period=10 * pq.ms, kernel=kernel)
    rates[key] = rate

mean_rates = []

for key in rates:
    mean_rates.append(np.mean(rates[key]))
    plt.plot(rates[key])

plt.show()

plt.hist(mean_rates, bins=20)
plt.show()