#%%
import numpy as np
import matplotlib.pyplot as plt
#%%
# Parameters
lower_bound_exc = 0
highest_bound_exc = 2
nb_syn = 10
nb_iter = 600000
inh_pot = 0.01
exc_pot = 0.005
noise = 0.000
max_considered_spread = 100
spread_factor = 1.5 # How much the 
shrink_factor = 3 # How much the 
# largest synapse possible gets its inhibitory potentiation divided 
#inh_pot_strenght = lambda weight : inh_pot/(((weight/ highest_bound_exc)*(inh_pot_div-1))+1)
#inh_pot_strenght = lambda weight : inh_pot-(weight/highest_bound_exc)*(inh_pot-(inh_pot/inh_pot_div))
spread = 0
thr_spread = 1
tau_cv_spread = 0.00001
#inh_pot_strenght = lambda weight,spread : inh_pot+(((spread/max_considered_spread)*(max_considered_spread/thr_spread))-1)*(weight/highest_bound_exc)*(inh_pot-(inh_pot/inh_pot_div))
inh_pot_strenght_spread = lambda weight : inh_pot-(weight/highest_bound_exc)*(inh_pot-(inh_pot/spread_factor))
inh_pot_strenght_shrink = lambda weight : inh_pot+(weight/highest_bound_exc)*(inh_pot-(inh_pot/shrink_factor))-(inh_pot-(inh_pot/spread_factor))


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
list_exc_w = []
spread_factor_list = []
#%%
i = 0
to_change= 0
which = 0
inh_pot_fun = inh_pot_strenght_spread
added = 0
while i<nb_iter:

    if i>60000:
        exc_syn_pool[0]+=0.000002
        added = 1

    mean_syn_w = np.mean(exc_syn_pool)
    spread = np.sum(np.abs(exc_syn_pool-mean_syn_w))

    if spread >thr_spread:
        spread_factor= max(1,spread_factor-0.001)
    else:
        spread_factor+=0.001
    cv_spread += (spread-cv_spread)*tau_cv_spread

    #if cv_spread>thr_spread and to_change == 0:
    if to_change == 0 and which == 0:
        print("changed")
        inh_pot_fun = inh_pot_strenght_shrink
        to_change = 400
        which = 1
    elif to_change == 0 :
        print("changed")
        inh_pot_fun = inh_pot_strenght_spread
        to_change = 400
        which = 0
        
    to_change -= 1 
    list_spread.append(spread)

    # cv_spread = (spread-cv_spread)/tau_cv_spread
    # if i%6000 == 0 or i ==0:
    #     print("spread:")
    #     print(spread)
    #     print("cv_spread:")
    #     print(cv_spread)
    #     # plt.hist(exc_syn_pool)
    #     # plt.show()

    exc_syn_pool += (np.random.rand(nb_syn)*noise)-noise/2
    diff = exc_syn_pool-inh_syn_pool
    to_visit=np.argmax(diff) # l'attracteur à visiter
    visited.append(to_visit)
    # print(to_visit)
    # print(diff[to_visit])
    exc_syn_pool[to_visit] = min(2,exc_syn_pool[to_visit]+exc_pot) # potentation of selected excitatory synapses
    exc_syn_pool = np.clip(exc_syn_pool-exc_pot/nb_syn,0,2) #normalization
    inh_pot_value = inh_pot_fun(inh_syn_pool[to_visit])
    # print(inh_pot_value)
    inh_syn_pool[to_visit] += inh_pot_value
    # print(inh_syn_pool[to_visit])
    inh_syn_pool -= inh_pot_value/nb_syn
    list_exc_w.append(exc_syn_pool)
    spread_factor_list.append(spread_factor)
    i+=1
# %%
plt.hist(exc_syn_pool,20)
# %%
plt.plot(list_spread)
#%%
list_exc_w= np.array(list_exc_w)
# %%
plt.plot(list_exc_w[-1000:])
# %%
plt.plot(list_exc_w)