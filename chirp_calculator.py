import numpy as np
import scipy
import matplotlib.pyplot as plt
from matplotlib import animation
from scipy import optimize

def v(t,name,width,peak):
    if name == 'puretone':
        return 0
    if name == 'square':
        if t>= peak - width/2 and t<= peak + width/2:
            return np.pi
        else:
            return 0

    if name == 'triangle':
        if t>=peak - width and t<=peak:
            return np.pi*(t/width + 1 - peak/width)
        elif t>=peak and t<= peak+width:
            return np.pi*(-t/width  + 1 + peak/width)
        else:
            return 0
    if name == 'gaussian':
        return np.pi*np.exp(-(t - peak)**2/2/width**2)

def normalize(y):
    newVals = []
    biggest = y[0]
    for i in range(len(y)):
        if y[i] > biggest:
            biggest = abs(y[i])
    if biggest != 0:
        for i in range(len(y)):
            newVals.append(y[i]/biggest)
        return newVals
    else:
        return y

w1 = 1
t_start = 0.
t_end = 1000.
timestep = .1
length = 100

def getMax(y,y2):
    m = y[0]
    for i in range(len(y)):
        if y[i] > m:
            m = y[i]
        if y2[i] > m:
            m = y2[i]
    return m

def getFFT(n):
    E = []
    E2 = []
    E3 = []
    time = []
    voltage = []
    t_start = 100*n + int(1E6)
    t_end = t_start + int(length)
    for t in range(t_start,t_end):
        voltage.append(v(timestep*float(t)))
        time.append(timestep*float(t))
        E.append(np.exp(1j*w1*2*np.pi*timestep*float(t) + 1j*v(timestep*float(t))) + np.exp(1j*w1*2*np.pi*timestep*float(t)))
        E2.append(np.exp(1j*w1*2*np.pi*timestep*float(t)) + np.exp(1j*w1*2*np.pi*timestep*float(t)))
        E3.append(np.exp(1j*w1*2*np.pi*timestep*float(t) + 1j*v(timestep*float(t))) + np.exp(1j*w1*2*np.pi*timestep*float(t) - 1j*v(timestep*float(t))))
    
    y = abs(np.fft.fft(E).real)
    y2 = abs(np.fft.fft(E3).real)
    freq = np.fft.fftfreq(len(time),d = timestep)
    return time,freq,y,y2

def plotFFT(n,ax,ax2,ax3,fig,pulseType):
    E = []
    E2 = []
    E3 = []
    E4 = []
    time = []
    voltage = []
    t_start =20*n + length
    t_end = t_start + int(length)
    for t in range(t_start,t_end):
        voltage.append(v(timestep*float(t),pulseType))
        time.append(timestep*float(t))
        E.append(np.exp(1j*w1*2*np.pi*timestep*float(t) + 1j*v(timestep*float(t),pulseType)) + np.exp(1j*w1*2*np.pi*timestep*float(t)))
        E2.append(np.exp(1j*w1*2*np.pi*timestep*float(t)) + np.exp(1j*w1*2*np.pi*timestep*float(t)))
        E3.append(np.exp(1j*w1*2*np.pi*timestep*float(t) + 0.5*1j*v(timestep*float(t),pulseType)) + np.exp(1j*w1*2*np.pi*timestep*float(t) - 0.5*1j*v(timestep*float(t),pulseType)))
        E4.append(abs(E[t - t_start]))
    y = abs(np.fft.fft(E).real)
    y2 = abs(np.fft.fft(E3).real)
    size = getMax(y,y2)
    if size == 0:
        ax2.set_ylim(0,1)
        ax.set_ylim(0,1)
    else:
        y = np.log(y)
        y2 = np.log(y2)
        ax2.set_ylim(0,1)
        ax.set_ylim(0,1)
        y = normalize(y)
        y2 = normalize(y2)
    freq = np.fft.fftfreq(len(time),d = timestep)
    ax2.plot(freq,y,'b-',label = 'z-cut')
    ax.plot(freq,y2,'r-',label = 'x-cut')
    ax.set_title(str(t_start*timestep) + " - " + str(t_end*timestep))
    ax.legend()
    ax2.legend()
    ax2.set_xlabel("Frequency")
    ax.set_ylabel("Log(FFT)")
    ax2.set_ylabel("Log(FFT)")
    ax3.plot(time,voltage,label = 'voltage')
    ax3.plot(time,E4,label = 'E Field')
    ax3.set_xlabel("time")
    ax3.set_ylabel("voltage")
    ax3.legend()
    ax3.set_ylim(0,4)

def update(n):
    if n >= 0 and (20*n + length)*timestep<=90:
        ax.cla()
        ax2.cla()
        ax3.cla()
        plotFFT(n,ax,ax2,ax3,fig,'gaussian')


def plotFFTCalc(n,ax,ax2,ax3,fig):
    ax.plot(freqs[n],zcut[n],label = 'zcut')
    ax2.plot(freqs[n],xcut[n],label = 'ycut')
    ax3.plot(time[n],v(time[n]))
    ax.legend()
    ax2.legend()
    plt.show()

def update2(n):
    if n >= 0 and (100*n + 1E-6 + length)*timestep<=4E-9:
        ax.cla()
        ax2.cla()
        ax3.cla()
        plotFFTCalc(n,ax,ax2,ax3,fig)

def timeAveragedFreqHist(t_start,t_stop,t_step,calc_length,skip_length,w,pulseType,Area,save):
    tempTime = []
    for i in range(int((t_stop - t_start)/t_step)):
        tempTime.append(t_start + float(i)*t_step)
    peak = mid(tempTime)
    if pulseType == 'square':
        width = Area
    elif pulseType == 'puretone':
        width = 0
    else:
        width = calculateWidth(Area,tempTime,t_step, pulseType)
    num_steps = int((t_stop - calc_length)/skip_length)
    print num_steps
    freq = np.fft.fftfreq(int(calc_length/t_step),d = t_step)
    avg1 = [0 for i in range(len(freq))]
    avg2 = [0 for i in range(len(freq))]
    for i in range(num_steps):
        time = []
        E1 = []
        E2 = []
        t1 = int(skip_length*i/t_step)
        t2 = int(t1 + calc_length/t_step)
        for t in range(t1,t2):
            time.append(t)
            #E1.append(np.exp(1j*w*2*np.pi*t_step*float(t) + 1j*v(t_step*float(t),pulseType,width,peak)) + np.exp(1j*w*2*np.pi*t_step*float(t)))
            E2.append(np.exp(1j*w*2*np.pi*t_step*float(t) + 0.5*1j*v(t_step*float(t),pulseType,width,peak)) + np.exp(1j*w*2*np.pi*t_step*float(t) - 0.5*1j*v(t_step*float(t),pulseType,width,peak)))
        #y1 = abs(np.fft.fft(E1))
        y2 = abs(np.fft.fft(E2))
        for i in range(len(freq)):
            #avg1[i] += y1[i]/num_steps
            avg2[i] += y2[i]/num_steps
    plt.title(pulseType)
    #plt.plot(freq,np.log(avg1),'b.',label = 'zcut')
    plt.plot(freq,np.log(avg2),'r.',label = 'xcut')
    plt.xlabel("Frequency")
    plt.ylabel("Log(E Field)")
    plt.legend()
    if save:
        plt.savefig("c:/users/abpoc/documents/fast_pulse_shaping/time_averaged_spectrum_" + pulseType + ".jpg")
    plt.show()

def mid(time):
    return min(time) + 0.5*(max(time) - min(time))

def err(w,Area,time,step,pulseType):
    A = 0.
    peak = mid(time)
    for t in time:
        A += np.sin(0.5*v(t,pulseType,w,peak))**2 * step
    return (A-Area)

def calculateWidth(Area,time,step, pulseType):
    pf,cov,info,mesg,success = optimize.leastsq(err,Area,args = (Area,time,step,pulseType),full_output = 1)
    return pf[0]


def f(t):
    


w = 300E14
Area = 5E-10
t_start = 0.
t_stop = 1E-11
calc_length = 1E-6
t_step = 1/2./w
skip_length = 1E-16
save = False


p = ['gaussian','puretone']
for pulseType in p:
    timeAveragedFreqHist(t_start,t_stop,t_step,calc_length,skip_length,w,pulseType,Area,save)

'''
Writer = animation.writers['ffmpeg']
writer = Writer(fps=5, metadata=dict(artist='Me'), bitrate=1800)

fig = plt.figure(figsize=(10,10))
ax = fig.add_subplot(3,1,1)
ax2 = fig.add_subplot(3,1,2)
ax3 = fig.add_subplot(3,1,3)
anim  = animation.FuncAnimation(fig, update,interval = 1)
#anim.save('c:/users/abpoc/documents/fast_pulse_shaping/gaussian_updated_pulse.mp4', writer=writer)
plt.show()
print "done"
'''
