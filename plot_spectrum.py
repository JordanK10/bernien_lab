import matplotlib.pyplot as plt
import numpy as np

Area = 1E-9
calc_length = 5E-9
w = 3.52E14
pulseType = "gaussian"
path  = "C:/Users/Bernien Lab/Desktop/fft2/1e-09ns_x_Intensity_200.0_MHz_resolution/square/"
freq,E = np.loadtxt(path + "6e-09.txt",unpack = True, skiprows = 1)
freq2 = []
E2 = []
for i in range(len(freq)):
    f = freq[i]
    if abs(f - w)<=2E9:
        freq2.append(f)
        E2.append(E[i])
    elif abs(f + w)<=2E9:
        freq2.append(abs(f))
        E2.append(E[i])

plt.plot(freq2,E2,'r.')
plt.yscale("Log")
plt.show()
