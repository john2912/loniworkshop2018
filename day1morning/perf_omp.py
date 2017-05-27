#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt

ds = np.loadtxt('perf_omp_strong.dat')
dw = np.loadtxt('perf_omp_weak.dat')
p = ds[:,0]
Tp_s = ds[:,1]
Tserial = Tp_s[0]

E_s = Tserial/(p*Tp_s)
Tp_w = dw[:,1]
E_w = Tserial/Tp_w

plt.ylim([0,1])
plt.xlabel('Number of processors (threads)')
plt.ylabel('Efficiency')
plt.plot(p,E_s,'r-o',label='strong scaling')
plt.plot(p,E_w,'g-s',label='weak scaling')
plt.legend()
plt.show()
