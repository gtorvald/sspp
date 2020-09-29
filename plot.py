import matplotlib.pyplot as plt
from array import *

data = []
for i in range(0, 6):
    time = 0
    for j in range(0, 10):
        time += float(input())
    time /= 10
    data.append(time)
plt.title("Time for 300x300")
plt.plot(["ijk", "ikj", "jik", "jki", "kij", "kji"], data)
plt.show()