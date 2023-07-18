from brian2 import *

time_sim = 10*second
nb_inputs = 10
w_input = 0.4

tau = 10*ms
tau_inh = 10*ms
tau_exc_inh = 200*ms
tau_x = 200*ms
tau_u = 600*ms
tau_self_exc = 1000*ms
tau_inh_exc = 100*ms
v_self_exc = 4
v_exc = 4
v_inh = -1
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

model_autapse= '''
dx/dt = (1-x)/tau_x :1
'''
on_pre_autapse = '''
self_exc+=x*Hz
x = x-0.04
'''

neurons = NeuronGroup(1, eqs, threshold='v>1', reset='v=0', method='euler',refractory=3*ms)
#inh_neurons = NeuronGroup(1, eqs_inh, threshold='v>1', reset='v=0', method='euler',refractory=0.3*ms)

stim_neurons_exc_syn = Synapses(network_inputs,neurons,on_pre="v+=w_input")
stim_neurons_exc_syn.connect()

#stim_neurons_inh_syn = Synapses(network_inputs,inh_neurons,on_pre="v+=w_input")
#stim_neurons_inh_syn.connect()

autapse = Synapses(neurons,neurons,model=model_autapse,on_pre=on_pre_autapse)
autapse.connect()

#inh_to_exc = Synapses(inh_neurons,neurons,on_pre="exc_inh+=0.7*Hz")
#inh_to_exc.connect()

#exc_to_inh = Synapses(neurons,inh_neurons,on_pre="inh_exc+=0.3*Hz")
#exc_to_inh.connect()

neurons_mon = StateMonitor(neurons, ["v", "self_exc"], record=[0])
spike_mon = SpikeMonitor(neurons)
#inh_rate_mon = PopulationRateMonitor(inh_neurons)
exc_rate_mon = PopulationRateMonitor(neurons)

run(time_sim, report='text', report_period=5 * second)

print(neurons_mon.v[0])
plt.plot(neurons_mon.v[0])
plt.show()

print(neurons_mon.self_exc[0])
plt.plot(neurons_mon.self_exc[0])
plt.show()

plt.eventplot(spike_mon.t)
plt.show()

plt.plot(exc_rate_mon.smooth_rate(window='gaussian',width=100*ms))
plt.show()

# plt.plot(inh_rate_mon.smooth_rate(window='gaussian',width=100*ms))
# plt.show()
