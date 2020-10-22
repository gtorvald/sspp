import matplotlib.pyplot as plt
from array import *

time_32ijk = []
time_32ikj = []
time_72ikj = []

L1_32ijk = []
L1_32ikj = []
L1_72ikj = []

CPU_32ijk = []
CPU_32ikj = []
CPU_72ikj = []

FLP_32ijk = []
FLP_32ikj = []
FLP_72ikj = []

for i in range(0, 5):
	data = float(input())
	time_32ijk.append(data)
	data = float(input())
	L1_32ijk.append(data)
	data = float(input())
	CPU_32ijk.append(data)
	data = float(input())
	FLP_32ijk.append(data)

for i in range(0, 5):
	data = float(input())
	time_32ikj.append(data)
	data = float(input())
	L1_32ikj.append(data)
	data = float(input())
	CPU_32ikj.append(data)
	data = float(input())
	FLP_32ikj.append(data)

for i in range(0, 5):
	data = float(input())
	time_72ikj.append(data)
	data = float(input())
	L1_72ikj.append(data)
	data = float(input())
	CPU_72ikj.append(data)
	data = float(input())
	FLP_72ikj.append(data)

plt.title("Time (PAPI_TOT_CYC)")
plt.plot(["1000", "2000", "3000", "4000", "5000"], time_32ijk, label='32 ijk')
plt.plot(["1000", "2000", "3000", "4000", "5000"], time_32ikj, label='32 ikj')
plt.plot(["1000", "2000", "3000", "4000", "5000"], time_72ikj, label='72 ikj')
plt.legend()
plt.show()

plt.title("L1 misses (PAPI_L1_DCM + PAPI_L1_ICM)")
plt.plot(["1000", "2000", "3000", "4000", "5000"], L1_32ijk, label='32 ijk')
plt.plot(["1000", "2000", "3000", "4000", "5000"], L1_32ikj, label='32 ikj')
plt.plot(["1000", "2000", "3000", "4000", "5000"], L1_72ikj, label='72 ikj')
plt.legend()
plt.show()

plt.title("Cycles (PAPI_TOT_CYC)")
plt.plot(["1000", "2000", "3000", "4000", "5000"], CPU_32ijk, label='32 ijk')
plt.plot(["1000", "2000", "3000", "4000", "5000"], CPU_32ikj, label='32 ikj')
plt.plot(["1000", "2000", "3000", "4000", "5000"], CPU_72ikj, label='72 ikj')
plt.legend()
plt.show()

plt.title("Flops (PAPI_FP_OPS)")
plt.plot(["1000", "2000", "3000", "4000", "5000"], FLP_32ijk, label='32 ijk')
plt.plot(["1000", "2000", "3000", "4000", "5000"], FLP_32ikj, label='32 ikj')
plt.plot(["1000", "2000", "3000", "4000", "5000"], FLP_72ikj, label='72 ikj')
plt.legend()
plt.show()