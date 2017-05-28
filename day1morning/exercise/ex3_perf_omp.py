#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt

ds = np.loadtxt('perf_omp_strg.dat')
dw = np.loadtxt('perf_omp_wk.dat')
p = ds[:,0].reshape(-1,1)
Tp_s = ds[:,1::2]
#print Tp_s
Tserial = Tp_s[0]
E_s = Tserial/(p*Tp_s)

Tp_w = dw[:,1::2]
Tserial = Tp_w[0]
E_w = Tserial/Tp_w

plt.ylim([0,1])
plt.subplot(2,1,1)
plt.plot(p,E_s,marker='s')
plt.xlabel('Number of processors (threads)')
plt.ylabel('Efficiency')
n=2**np.arange(ds.shape[1]+1)
str_lgd = ['strong n='+str(_n) for _n in n]
plt.legend(str_lgd)
plt.subplot(2,1,2)
plt.ylim([0,1])
plt.plot(p,E_w,marker='o')
plt.xlabel('Number of processors (threads)')
plt.ylabel('Efficiency')
n=2**np.arange(dw.shape[1]+1)
str_n = ['weak n='+str(_n)+'x' for _n in n]
plt.legend(str_n)
plt.show()
