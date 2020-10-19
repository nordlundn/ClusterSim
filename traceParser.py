import numpy as np
from tqdm import tqdm
import pandas as pd
import pickle as pkl
import gzip
import matplotlib.pyplot as plt

SUBMIT = 0
SCHEDULE = 1
EVICT = 2
FAIL = 3
FINISH = 4
KILL = 5
LOST = 6

taskTable = {}
#element [arrivalTime, scheduledTime, departureTime, mem, cpu]

maxTime = 0

def fillTaskTable(filename):
    # data = pd.read_csv(filename, compression='gzip', error_bad_lines=False)
    # for index, row in data.iterrows():
    with gzip.open(filename, 'rb') as f:
        contents = f.readlines()


    for row in contents:
        row = row.decode('utf8').strip().split(',')
        missingInfo = row[1]
        timeStamp = row[0]
        timeStamp = int(np.round(float(timeStamp)/1e6))

        # if missingInfo == '' and timeStamp > 0:
        if missingInfo == '':

            jobID = int(row[2])
            taskID = int(row[3])
            eventType = int(row[5])
            cpu = row[9]
            mem = row[10]
            mem = int(np.round(float(mem)*256))
            cpu = int(np.round(float(cpu)*64))

            taskName = (jobID, taskID)
            if eventType == SUBMIT:
                taskTable[taskName] = -np.ones(5)
                taskTable[taskName][0] = timeStamp
                taskTable[taskName][3] = cpu
                taskTable[taskName][4] = mem

            if eventType == SCHEDULE and taskName in taskTable.keys():
                taskTable[taskName][1] = timeStamp
            if eventType >= SCHEDULE and eventType <= LOST  and taskName in taskTable.keys():
                taskTable[taskName][2] = timeStamp

for i in tqdm(range(0,500)):
    path = "/Users/nick/Documents/Yale/BestFit/clusterdata-2011-2/task_events/"
    file1 = "part-"
    file2 = str(i).zfill(5)
    file3 = "-of-00500.csv.gz"

    filename = path+file1+file2+file3
    fillTaskTable(filename)

valid = 0
notValid = 0
jobsBySecond = {}


for taskName in tqdm(taskTable.keys()):
    task = taskTable[taskName]
    timeStamp = task[0]
    if np.any(task < 0):
        notValid += 1
        continue
    valid += 1

    duration = np.max([(task[2] - task[1]), 1])
    mem = task[4]
    cpu = task[3]
    newTask = (taskName, mem, cpu, duration)

    if timeStamp not in jobsBySecond.keys():
        jobsBySecond[timeStamp] = []

    if timeStamp > maxTime:
        maxTime = timeStamp
    jobsBySecond[timeStamp].append(newTask)

# with open("jobsBySecond.pkl", 'wb') as f:
#     pkl.dump(jobsBySecond, f)

print(valid, notValid)

mems = []
durs = []
nums = []
cpus = []
arrivals = []

BLOCK_SIZE = 256

for i in tqdm(range(int(maxTime)+1)):
    if i in jobsBySecond.keys():
        tasksList = jobsBySecond[i]
        nums.append(len(tasksList))
        for task in tasksList:
            taskName, mem, cpu, dur = task
            if mem > 0 and dur > 0 and cpu > 0:
                mems.append(mem)
                durs.append(dur)
                cpus.append(cpu)
                arrivals.append(i)


mems = np.array(mems, dtype=int)
durs = np.array(durs, dtype=int)
cpus = np.array(cpus, dtype=int)
arrivals = np.array(arrivals, dtype=int)


# plt.hist(mems)
# plt.show()
#
# plt.hist(durs)
# plt.show()
#
# plt.hist(cpus)
# plt.show()
#
# plt.hist(arrivals)
# plt.show()

print("Saving nums...")
with open("num.pkl", "wb") as f:
    pkl.dump(nums, f)
print("Done!")
print("Saving mems...")
with open("mems.pkl", "wb") as f:
    pkl.dump(mems, f)
print("Done!")
print("Saving durs...")
with open("durs.pkl", "wb") as f:
    pkl.dump(durs, f)
print("Saving cpus...")
with open("cpus.pkl", "wb") as f:
    pkl.dump(cpus, f)
print("Done!")
print("Saving arrival times...")
with open("times.pkl", "wb") as f:
    pkl.dump(arrivals, f)
print("Done!")


final = []

for t, mem, cpu, dur in zip(arrivals, mems, cpus, durs):
    final.append([t,mem,cpu,dur])
final = np.array(final).astype(int)

np.savetxt("taskProfile.csv", final, delimiter=',', newline='\n', encoding=None, fmt='%d')

print("Ready to rock!")
