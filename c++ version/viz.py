
#%%
import numpy as np
from matplotlib import pyplot as plt

data = np.loadtxt("activity_history.txt")

#%%

plt.plot(data.T)
plt.show()

# %%
# plt.plot(data[0,:])
# plt.plot(data[1,:])
# plt.plot(data[4,:])
# plt.plot(data[5,:])
# plt.show()
# %%
