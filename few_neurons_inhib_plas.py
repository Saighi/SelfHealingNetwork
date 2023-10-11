from brian2 import *

time_sim = 10*second
nb_inputs = 10

tau = 10*ms
tau_ampa = 5*ms
tau_nmda = 100*ms
tau_gaba = 50*ms

eqs = '''
dgaba/dt = -gaba/tau_gaba : Hz
dnmda/dt = -nmda/tau_nmda : Hz
dv/dt = (-v/tau) + (self_exc*(v_self_exc-v))+(exc_inh*(v_inh-v)) : 1
'''

