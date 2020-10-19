import numpy as np
import pickle
import matplotlib.pyplot as plt
import EALA
from tqdm import tqdm

# taskProfile = np.genfromtxt("taskProfile.csv", delimiter=',')

with open("taskProfile.csv", "rb") as f:
    contents = f.readlines()


def cleanLine(line):
    return list(map(int, line.decode('utf-8').strip().split(",")))

lastLine = cleanLine(contents[-1])
maxT = lastLine[0]

actives = []
finished = []
utilization = []

cloud = EALA.Cloud()

clock = -1
i = 0
runs = 50000

avgUtilization = np.zeros([7000])
alpha = .99

# while i < len(contents):

pbar = tqdm(total = runs+1)
while clock < runs:
    next_clock, mem, cpu, dur = cleanLine(contents[i])
    while clock < next_clock:
        finished.append(cloud.tick())
        cloud.schedule(1)
        actives.append(cloud.getNumActive())
        utilization.append(cloud.getUtilization())
        c = cloud.getCloud()
        c = 128 - c[:7000]

        avgUtilization = avgUtilization*alpha + (1-alpha)*c/128.0 
        clock += 1
        pbar.update(1)
    
    if mem > 128:
        mem = 128

    cloud.add(mem,dur)
    i+=1

pbar.close()
print(np.mean(utilization))
print(cloud.getNumFinished())
print(cloud.getMemFinished())

plt.subplot(1,2,1)
plt.plot(actives)
plt.subplot(1,2,2)
plt.plot(avgUtilization)
plt.show()