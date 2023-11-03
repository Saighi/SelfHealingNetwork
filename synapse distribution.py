#%%
import numpy as np
import matplotlib.pyplot as plt
#%%
# Parameters
lower_bound_exc = 0
highest_bound_exc = 2
nb_syn = 200
nb_iter = 2400000
inh_pot = 0.01
exc_pot = 0.002
noise = 0.0001
max_considered_spread = 100
inh_pot_div = 3 # How much the 
# largest synapse possible gets its inhibitory potentiation divided 
#inh_pot_strenght = lambda weight : inh_pot/(((weight/ highest_bound_exc)*(inh_pot_div-1))+1)
#inh_pot_strenght = lambda weight : inh_pot-(weight/highest_bound_exc)*(inh_pot-(inh_pot/inh_pot_div))
spread = 0
thr_spread = 70
tau_cv_spread = 0.00001
#inh_pot_strenght = lambda weight,spread : inh_pot+(((spread/max_considered_spread)*(max_considered_spread/thr_spread))-1)*(weight/highest_bound_exc)*(inh_pot-(inh_pot/inh_pot_div))
inh_pot_strenght = lambda weight,target : inh_pot+target*(weight/highest_bound_exc)*(inh_pot-(inh_pot/inh_pot_div))

#%%
exc_syn_pool = 1+(np.random.rand(nb_syn)-0.25)/5
mean_syn_w = np.mean(exc_syn_pool)
spread = np.sum(np.abs(exc_syn_pool-mean_syn_w))
cv_spread=spread
inh_syn_pool = np.full(nb_syn,1.0)
#%%
plt.hist(exc_syn_pool,20)
visited = []
list_spread = []
#%%
i = 0
target = 0
while i<nb_iter:

    mean_syn_w = np.mean(exc_syn_pool)
    spread = np.sum(np.abs(exc_syn_pool-mean_syn_w))
    cv_spread += (spread-cv_spread)*tau_cv_spread

    if i<1600000:
        if cv_spread>thr_spread:
            target = 1
        else :
            target = -1
    else :
        if cv_spread>thr_spread:
            target = 1.5
        else :
            target = -1.5

    list_spread.append(spread)

    # cv_spread = (spread-cv_spread)/tau_cv_spread
    if i%6000 == 0 or i ==0:
        print("spread:")
        print(spread)
        print("cv_spread:")
        print(cv_spread)
        # plt.hist(exc_syn_pool)
        # plt.show()

    exc_syn_pool += (np.random.rand(nb_syn)*noise)-noise/2
    diff = exc_syn_pool-inh_syn_pool
    to_visit=np.argmax(diff) # l'attracteur à visiter
    visited.append(to_visit)
    # print(to_visit)
    # print(diff[to_visit])
    exc_syn_pool[to_visit] = min(2,exc_syn_pool[to_visit]+exc_pot) # potentation of selected excitatory synapses
    exc_syn_pool = np.clip(exc_syn_pool-exc_pot/nb_syn,0,2) #normalization
    inh_pot_value = inh_pot_strenght(inh_syn_pool[to_visit],target)
    # print(inh_pot_value)
    inh_syn_pool[to_visit] += inh_pot_value
    # print(inh_syn_pool[to_visit])
    inh_syn_pool -= inh_pot_value/nb_syn
    # print(inh_syn_pool)
    i+=1
# %%
plt.hist(exc_syn_pool,20)
# %%
plt.plot(list_spread)