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
    for i in range(num_steps):
        print("Step: ",i)
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
        if save:
            writeToText(path,freq,y2,str(0.5*t_step*(t1+t2)))
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
Area = 1E-9         ### area under tha pulse that you want, in units of W*ns
t_start = 0.        ### start time - probs just leave this at 0
t_stop = 1E-8       ### ending time - make sure that it is long enough to get the whole pulse
                    ### a good rule of thumb is to estimate the pulse width at approximately the
                    ### the same as the area, and then double that. the pulse is automatically
                    ### centered in the time frame
calc_length = 5E-9  ### length of each time chunk to be fft'ed. Longer calc lengths mean higher
                    ### better frequency resolution, but they also mean your gettig more global
                    ### and less local data.
t_step = 1/2./w     ### time step increment. This is the maximum value to avoid nyquist sampling.
                    ### much shorter and the calculations will get very long
skip_length = 1E-10 ### how far in time you skip in  between each fft. this defines the time grain
                    ### when you look at frequency as a function of time
save = True         ### binary, if you want to save the data
show = False        ### binary, if you want to see the data


###################Done###########################################

def pulseTrain(t,clock_rate):
    step = 1./clock_rate
    sigma = 2.*step
    v = 0.
    centers = [20.*step*float(i) for i in range(51)]
    for c in centers:
        v += np.pi/4*np.exp(-(t-c)**2/2/sigma**2)
    return v

def singlePulse(t,clock_rate):
    step = 1./clock_rate
    sigma = 2.*step
    v = 0.
    centers = [100.*step]
    for c in centers:
        v += np.pi/4*np.exp(-(t-c)**2/2/sigma**2)
    return v

def noTails(t,clock_rate):
    step = 1./clock_rate
    sigma = 2.*step
    v = 0.
    centers = [5.*step]
    for c in centers:
        v += np.pi/4*np.exp(-(t-c)**2/2/sigma**2)
    return v

def longTails(t,clock_rate):
    step = 1./clock_rate
    sigma = 2.*step
    v = 0.
    centers = [250.*step]
    for c in centers:
        v += np.pi/2*np.exp(-(t-c)**2/2/sigma**2)
    return v

def g(center,sigma,t):
    return np.exp(-(t - center)**2/2/sigma**2)

def getModulationSingle(time,clock_rate):
    return np.exp(1j*singlePulse(time,clock_rate)) + np.exp(-1j*singlePulse(time,clock_rate) + 1j*np.pi)

def getModulationNoTails(time,clock_rate):
    return np.exp(1j*noTails(time,clock_rate)) + np.exp(-1j*noTails(time,clock_rate) + 1j*np.pi)

def getModulationLongTails(time,clock_rate):
    return np.exp(1j*longTails(time,clock_rate)) + np.exp(-1j*longTails(time,clock_rate) + 1j*np.pi)

def getModulationTrain(time,clock_rate):
    return np.exp(1j*pulseTrain(time,clock_rate)) + np.exp(-1j*pulseTrain(time,clock_rate) + 1j*np.pi)

def pureTone(w,t):
    return np.exp(1j*w*2*np.pi*t)

def singlePulseWithWidth(w,clock_rate,starting_width,path):
    t_step = 1./4./w
    num_points = int(250./4e9/t_step)
    print("Total time points: " + str(num_points))
    time = np.linspace(0,250./4e9,num_points)
    modulation = getModulationSingle(time,clock_rate)
    freqs = np.fft.fftfreq(num_points,t_step)
    if starting_width>0:
        E_f = g(w,starting_width,freqs)
        print("E field constructed in frequency domain")
        E_start = np.fft.ifft(E_f)
        print("E converted to time domain")
    else:
        E_start = pureTone(w,time)
        print("E Field Constructed")
    E = E_start*modulation
    print("E Field modulated in time. Beginning FFT...")
    E_f2 = np.fft.fft(E)
    plt.plot(freqs,E_f2,'k.')
    plt.xlim(w-2e9,w+2e9)
    plt.title("Single Pulse w/ " + str(starting_width) + "Hz startng Line Width")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("E Field Strength (a.u.)")
    plt.savefig(path + "single_pulse_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width.jpg")
    plt.savefig(path + "single_pulse_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width.pdf")
    plt.close()
    writeToText(path,freqs,E_f2,"single_pulse_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width")

def singlePulseWithWidthNoTails(w,clock_rate,starting_width,path):
    t_step = 1./10./w
    num_points = int(10./4e9/t_step)
    print("Total time points: " + str(num_points))
    time = np.linspace(0,250./4e9,num_points)
    modulation = getModulationNoTails(time,clock_rate)
    freqs = np.fft.fftfreq(num_points,t_step)
    if starting_width>0:
        E_f = g(w,starting_width,freqs)
        print("E field constructed in frequency domain")
        E_start = np.fft.ifft(E_f)
        print("E converted to time domain")
    else:
        E_start = pureTone(w,time)
        print("E Field Constructed")
    E = E_start*modulation
    print("E Field modulated in time. Beginning FFT...")
    E_f2 = np.fft.fft(E)
    plt.plot(freqs,E_f2,'k.')
    plt.xlim(w-2e9,w+2e9)
    plt.title("Single Pulse (No Tails) w/ " + str(starting_width) + "Hz startng Line Width")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("E Field Strength (a.u.)")
    plt.savefig(path + "single_pulse_no_tails_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width.jpg")
    plt.savefig(path + "single_pulse_no_tails_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width.pdf")
    plt.close()
    writeToText(path,freqs,E_f2,"single_pulse_no_tails_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width")

def singlePulseWithWidthLongTails(w,clock_rate,starting_width,path):
    t_step = 1./4./w
    num_points = int(500./4e9/t_step)
    print("Total time points: " + str(num_points))
    time = np.linspace(0,500./4e9,num_points)
    modulation = getModulationLongTails(time,clock_rate)
    freqs = np.fft.fftfreq(num_points,t_step)
    if starting_width>0:
        E_f = g(w,starting_width,freqs)
        print("E field constructed in frequency domain")
        E_start = np.fft.ifft(E_f)
        print("E converted to time domain")
    else:
        E_start = pureTone(w,time)
        print("E Field Constructed")
    E = E_start*modulation
    print("E Field modulated in time. Beginning FFT...")
    E_f2 = np.fft.fft(E)
    plt.plot(freqs,E_f2,'k.')
    plt.xlim(w-2e9,w+2e9)
    plt.title("Single Pulse (Long Tails) w/ " + str(starting_width) + "Hz startng Line Width")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("E Field Strength (a.u.)")
    plt.savefig(path + "single_pulse_long_tails_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width.jpg")
    plt.savefig(path + "single_pulse_long_tails_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width.pdf")
    plt.close()
    reduced_E = []
    new_freqs = []
    for i in range(len(freqs)):
        if freqs[i]>=w-2e9 and freqs[i]<=w+2e9:
            new_freqs.append(freqs[i])
            reduced_E.append(E_f2[i])
    writeToText(path,new_freqs,reduced_E,"single_pulse_long_tails_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width")

def pulseTrainWithWidth(w,clock_rate,starting_width,path):
    t_step = 1./4./w
    num_points = int(250./4e9/t_step)
    print("Total time points: " + str(num_points))
    time = np.linspace(0,250./4e9,num_points)
    modulation = getModulationTrain(time,clock_rate)
    freqs = np.fft.fftfreq(num_points,t_step)
    if starting_width>0:
        E_f = g(w,starting_width,freqs)
        print("E field constructed in frequency domain")
        E_start = np.fft.ifft(E_f)
        print("E converted to time domain")
    else:
        E_start = pureTone(w,time)
        print("E Field Constructed")
    E = E_start*modulation
    print("E Field modulated in time. Beginning FFT...")
    E_f2 = np.fft.fft(E)
    plt.plot(freqs,E_f2,'k.')
    plt.xlim(w-2e9,w+2e9)
    plt.title("Pulse Train w/ " + str(starting_width) + "Hz startng Line Width")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("E Field Strength (a.u.)")
    plt.savefig( path + "pulse_train_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width.jpg")
    plt.savefig(path + "pulse_train_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width.pdf")
    plt.close()
    writeToText(path,freqs,E_f2,"pulse_train_fft_" + str(clock_rate/1e9) + "GSs_" + str(starting_width) + "Hz_line_width")

def extractRelevantData(path,path2,freq1,freq2):
    freq,E = np.loadtxt(path + ".txt",unpack = True,skiprows = 1, dtype=complex, converters={0: lambda s: complex(s.decode().replace('+-', '-'))})
    f = open(path2 + "reduced.txt",'w')
    f.write("freq\tE\n")
    for i in range(len(freq)):
        if float(freq[i]) >= freq1 and freq[i] <= freq2:
            f.write(str(freq[i]) + "\t" + str(E[i]) + "\n")
    f.close()
"""
path  = "c:/users/bernien lab/desktop/pulse_ffts_variable_clock_rate_2/"
#os.mkdir(path)
for clock_rate in [2e9,3e9,4e9]:
    for sigma in [0,2e5,5e5,1e6]:
        if clock_rate == 2e9:
            continue
        if clock_rate == 3e9:
            continue
        if clock_rate == 4e9 and sigma == 0:
            continue
        if clock_rate == 4e9 and sigma == 2e5:
            pulseTrainWithWidth(w,clock_rate,sigma,path)
        else:
            singlePulseWithWidthNoTails(w,clock_rate,sigma,path)
            singlePulseWithWidth(w,clock_rate,sigma,path)
            pulseTrainWithWidth(w,clock_rate,sigma,path)
"""

#SAMPLE WITH lONG Tails
path  = "c:/users/bernien lab/desktop/long_sample/"
#os.mkdir(path)
for clock_rate in [3e9,4e9]:
    for sigma in [0,2e5,5e5]:
        singlePulseWithWidthLongTails(w,clock_rate,sigma,path)

"""
path  = "c:/users/bernien lab/desktop/pulse_ffts_variable_clock_rate_2/"
path3 = "c:/users/bernien lab/desktop/pulse_ffts_reduced_txt/"
os.mkdir(path3)
freq1  = w - 2e9
freq2 = w + 2e9
for clock_rate in [2e9,3e9,4e9]:
    for sigma in [0,2e5,5e5,1e6]:
        print(sigma)
        path2 = path + "pulse_train_fft_" + str(clock_rate/1e9) + "GSs_" + str(sigma) + "Hz_line_width"
        path4  =path3 + "pulse_train_fft_" + str(clock_rate/1e9) + "GSs_" + str(sigma) + "Hz_line_width"
        extractRelevantData(path2,path4,freq1,freq2)
        path2 = path + "single_pulse_fft_" + str(clock_rate/1e9) + "GSs_" + str(sigma) + "Hz_line_width"
        path4  =path3 + "single_pulse_fft_" + str(clock_rate/1e9) + "GSs_" + str(sigma) + "Hz_line_width"
        extractRelevantData(path2,path4,freq1,freq2)
        path2 = path + "single_pulse_no_tails_fft_" + str(clock_rate/1e9) + "GSs_" + str(sigma) + "Hz_line_width"
        path4  =path3 + "single_pulse_no_tails_fft_" + str(clock_rate/1e9) + "GSs_" + str(sigma) + "Hz_line_width"
        extractRelevantData(path2,path4,freq1,freq2)
"""

"""
for i in range(2):
    p = ['gaussian','truegaussian','square','puretone']
    path  = "c:/users/bernien lab/desktop/fft2/" + str(Area) + "ns_x_Intensity_" + str(1/(calc_length)/1E6) + "_MHz_resolution/"
    if os.path.isdir(path):
        a = input("This already Exists. Would you like to overwrite? (y/n)")
        if a == "y":
            for pulseType in p:
                timeAveragedFreqHist(t_start,t_stop,t_step,calc_length,skip_length,w,pulseType,Area,path + pulseType + "/" ,save,show)
    else:
        os.mkdir(path)
        for pulseType in p:
            timeAveragedFreqHist(t_start,t_stop,t_step,calc_length,skip_length,w,pulseType,Area,path  + pulseType + "/",save,show)
    Area = 5E-10
"""
