import numpy as np
import scipy
import matplotlib.pyplot as plt
from matplotlib import animation
from scipy import optimize
import os

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
    if name == 'gaussian' or 'truegaussian':
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

def writeToText(path,freq,fft,fname):
    if os.path.isfile(path+fname+".txt"):
        os.remove(path+fname+".txt")
    f = open(path+fname+".txt",'w')
    f.write("freq\tE\n")
    for i in range(len(freq)):
        f.write(str(freq[i]) + "\t" + str(fft[i]) + "\n")
    f.close()

def saveParameters(t_start,t_stop,t_step,calc_length,skip_length,w,pulseType,Area,width,path):
    if os.path.isfile(path+"settings.txt"):
        os.remove(path+"settings.txt")
    f = open(path+"settings.txt",'w')
    f.write("t_start: " + str(t_start) + "\n")
    f.write("t_stop: " + str(t_stop) + "\n")
    f.write("calc_length: " + str(calc_length) + "\n")
    f.write("skip_length: " + str(skip_length) + "\n")
    f.write("frequency: " + str(w) + "\n")
    f.write("pulse type: " + pulseType + "\n")
    f.write("Area: " + str(Area) + "\n")
    f.write("width: " + str(width) + "\n")
    f.close()

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

def timeAveragedFreqHist(t_start,t_stop,t_step,calc_length,skip_length,w,pulseType,Area,path,save,show):
    print("Started")
    if save:
        if not os.path.isdir(path):
            os.mkdir(path)
    tempTime = []
    tempFactor = 200
    print(int((t_stop - t_start)/t_step)/1E3/tempFactor," thousand time steps")
    for i in range(int((t_stop - t_start)/t_step/tempFactor)):
        tempTime.append(t_start + float(i)*t_step*tempFactor)
    print("Temp Time Done")
    peak = mid(tempTime)
    print("peak: ", peak)
    if pulseType == 'square':
        width = Area
    elif pulseType == 'puretone':
        width = 0
    elif pulseType == 'truegaussian':
        width = Area/np.sqrt(2*np.pi)
    else:
        width = calculateWidth(Area,tempTime,t_step*tempFactor, pulseType)
    if save:
        saveParameters(t_start,t_stop,t_step,calc_length,skip_length,w,pulseType,Area,width,path)
    print("width: ", width)
    print("Frequency Grain = ", 1/(calc_length)/1E6, " MHz")
    temptime = []
    num_steps = int((t_stop - calc_length)/skip_length)
    print(num_steps, " total time steps")
    print("starting ffts")
    freq = np.fft.fftfreq(int(calc_length/t_step),d = t_step)
    avg1 = [0 for i in range(len(freq))]
    avg2 = [0 for i in range(len(freq))]
    time = []
    center_freq = []
    centroid = []
    E_field = []
    Voltage = []
    currTime = []
    for i in range(num_steps):
        print("Step: ",i)
        E2 = []

        t1 = int(skip_length*i/t_step)
        t2 = int(t1 + calc_length/t_step)
        for t in range(t1,t2):
            currVoltage = v(t_step*float(t),pulseType,width,peak)
            currTime.append(float(t)*t_step)
            Voltage.append(currVoltage)
            curr_E = np.exp(1j*w*2*np.pi*t_step*float(t) + 0.5*1j*currVoltage) - np.exp(1j*w*2*np.pi*t_step*float(t) - 0.5*1j*currVoltage)
            E2.append(curr_E)
            E_field.append(abs(curr_E))
        y2 = abs(np.fft.fft(E2))
        for i in range(len(freq)):
            avg2[i] += y2[i]/num_steps
        if save:
            writeToText(path,freq,y2,str(0.5*t_step*(t1+t2)))
        time.append(0.5*t_step*(t1+t2))
        centroid.append(centerFreq(freq,y2))
        center_freq.append(getPeak(freq,y2))
    if save:
        writeToText(path,time,centroid,"centroid_frequencies")
        writeToText(path,time,center_freq,"peak_frequencies")
        writeToText(path,currTime,E_field,"E_vs_Time")
        writeToText(path,currTime,Voltage,"Voltage_vs_Time")


    plt.title(pulseType)
    #plt.plot(freq,np.log(avg1),'b.',label = 'zcut')
    plt.plot(freq,np.log(avg2),'r.',label = 'xcut')
    plt.xlabel("Frequency")
    plt.ylabel("Log(E Field)")
    plt.legend()
    if save:
        writeToText(path,freq,avg2,"Time Average Data")
        plt.savefig(path + "time_average.pdf")
    if show:
        plt.show()

def mid(time):
    return min(time) + 0.5*(max(time) - min(time))

def err(w,Area,time,step,pulseType):
    A = 0.
    peak = mid(time)
    for t in time:
        A += np.sin(0.5*v(t,pulseType,w,peak)) * step
    return (A-Area)

def calculateWidth(Area,time,step, pulseType):
    pf,cov,info,mesg,success = optimize.leastsq(err,Area,args = (Area,time,step,pulseType),full_output = 1)
    return pf[0]

##########Control Parameters###########################

w = 3.52E14         ### frequency of 852 nm light
Area = 1E-9         ### area under tha pulse that you want, in units of E*s
t_start = 0.        ### start time - probs just leave this at 0
t_stop = 1E-8       ### ending time - make sure that it is long enough to get the whole pulse
                    ### a good rule of thumb is to estimate the pulse width at approximately the
                    ### the same as the area, and then double that. the pulse is automatically
                    ### centered in the time frame
calc_length = 1E-10  ### length of each time chunk to be fft'ed. Longer calc lengths mean higher
                    ### better frequency resolution, but they also mean your gettig more global
                    ### and less local data.
t_step = 1/4./w     ### time step increment. This is the maximum value to avoid nyquist sampling.
                    ### much shorter and the calculations will get very long
skip_length = 5E-11 ### how far in time you skip in  between each fft. this defines the time grain
                    ### when you look at frequency as a function of time
save = True         ### binary, if you want to save the data
show = False        ### binary, if you want to see the data


###################Done###########################################


p = ['gaussian','truegaussian','square','puretone']
path  = "c:/users/bernien lab/desktop/fft3/" + str(Area) + "ns_x_Intensity_" + str(round(1/(calc_length)/1E6,0)) + "_MHz_resolution/"
if os.path.isdir(path):
    a = input("This already Exists. Would you like to overwrite? (y/n)")
    if a == "y":
        for pulseType in p:
            timeAveragedFreqHist(t_start,t_stop,t_step,calc_length,skip_length,w,pulseType,Area,path + pulseType + "/" ,save,show)
else:
    os.mkdir(path)
    for pulseType in p:
        timeAveragedFreqHist(t_start,t_stop,t_step,calc_length,skip_length,w,pulseType,Area,path  + pulseType + "/",save,show)
