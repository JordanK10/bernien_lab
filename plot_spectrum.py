import matplotlib.pyplot as plt
import numpy as np

def centerFreq(freq,E):
    COM = 0.0
    tot = 0.0
    for i in range(len(freq)):
        COM += freq[i]*E[i]
        tot += E[i]
    return COM/tot

def getPeak(freq,E):
    peakfreq = freq[0]
    peakE = E[0]
    for i in range(len(freq)):
        if E[i]>peakE:
            peakE = E[i]
            peakfreq = freq[i]
    return peakfreq
    
Area = 1E-9
calc_length = 5E-9
w = 3.52E14
skip_length = 1E-10
t_stop = 1E-8
t_step = 1/2./w
cutOff = 5E9
pulseType = "gaussian"
path  = "C:/Users/Bernien Lab/Desktop/fft2/1e-09ns_x_Intensity_499.99999999999994_MHz_resolution/gaussian/"
num_steps = int((t_stop - calc_length)/skip_length)
time = []
shifted_freq = []
peak_freq = []
for i in range(num_steps):
    print(i)
    t1 = int(skip_length*i/t_step)
    t2 = int(t1 + calc_length/t_step)
    fname = str(0.5*t_step*(t1+t2))
    freq,E = np.loadtxt(path + fname + ".txt",unpack = True, skiprows = 1)
    freq2 = []
    E2 = []
    for i in range(len(freq)):
        f = freq[i]
        if abs(f - w)<=cutOff:
            freq2.append(f)
            E2.append(E[i])
        elif abs(f + w)<=cutOff:
            freq2.append(w + abs(f+w))
            E2.append(E[i])
    time.append(0.5*t_step*(t1+t2))
    shifted_freq.append(centerFreq(freq2,E2))
    peak_freq.append(getPeak(E2))

plt.plot(time,shifted_freq,'r.',label = 'centroid')
plt.plot(time,peak_freq,'b.',label = "max")
plt.legend
plt.show()
