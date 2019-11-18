import matplotlib.pyplot as plt
import numpy as np


path  = "C:/Users/Bernien Lab/Desktop/fft2/1e-09ns_x_Intensity_499.99999999999994_MHz_resolution/gaussian/"
time,peak_freq = np.loadtxt(path + "peak_frequencies.txt",unpack = True,skiprows = 1)
time,centroid_freq = np.loadtxt(path + "centroid_frequencies.txt",unpack = True,skiprows = 1)

plt.plot(time,centroid_freq,'r.',label = 'centroid')
plt.plot(time,peak_freq,'b.',label = "max")
plt.legend()
plt.ylabel("Frequency")
plt.xlabel("Time")
plt.savefig(path + "freq_v_time.pdf")
plt.show()
