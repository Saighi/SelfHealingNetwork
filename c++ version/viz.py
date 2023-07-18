
#%%
import numpy as np
from matplotlib import pyplot as plt

data = np.loadtxt("activity_history.txt")

#%%

plt.plot(data.T)


# %%

plt.plot(data[1,:])
# %%
