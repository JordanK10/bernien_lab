import matplotlib.pyplot as plt
import numpy as np
import os

def plot_freqs():
    directory  = "C:/Users/Bernien Lab/Desktop/fft3/"
    folders = ["5E-10ns_x_Intensity_10000.0_MHz_resolution/"]
    fname = "/peak_frequencies.txt"
    time = []
    freq = []
    names = []
    w = 3.52E14
    t_step = 1/4./w
    norm_freq = []
    count = 0
    s = 93
    e = 105
    colors = ['r.','b.','g.','k.']
    for path in folders:
        for subfolder in os.listdir(directory + path):
            if subfolder[-3:] == "pdf":
                continue
            if subfolder != 'puretone':
                time.append([])
                freq.append([])
                names.append(subfolder)
                time[count],freq[count] = np.loadtxt(directory + path + subfolder + fname,unpack = True, skiprows = 1)
                count += 1
            else:
                t,norm_freq = np.loadtxt(directory + path + subfolder + fname,unpack = True, skiprows = 1)
        for i in range(len(time)):
            #plt.plot(t,norm_freq)
            plt.plot(time[i][s:e],(freq[i][s:e] - w)/w,colors[i],label = names[i])
        plt.legend()
        plt.xlabel("time")
        plt.title("100ps calc length")
        plt.ylabel("Percent Frequency Change (Peak)")
        plt.savefig(directory + path + "peak_freq.pdf")
        plt.show()

def plot_fields():
    directory  = "C:/Users/Bernien Lab/Desktop/fft3/"
    for folders in os.listdir(directory):
        if folders == "5E-10ns_x_Intensity_10000.0_MHz_resolution/":
            continue
        fnames = ["/Voltage_vs_Time.txt","/E_vs_Time.txt"]
        for subfolder in os.listdir(directory + folders):
            if subfolder[-3:] == "pdf":
                continue
            elif subfolder == 'puretone':
                continue
            else:
                print("Currently Working on: " + folders+"/"+subfolder)
                t1,E = np.loadtxt(directory + folders + "/" + subfolder + fnames[1],unpack = True, skiprows = 1)
                t2,V = np.loadtxt(directory + folders + "/" + subfolder + fnames[0],unpack = True, skiprows = 1)
            plt.plot(t1,E,label = "E Field")
            plt.plot(t2,V,label = "Voltage")
            plt.legend()
            plt.xlabel("time")
            plt.ylabel("Relative strength (a.u.)")
            plt.savefig(directory+path+subfolder+"/E_V_vs_Time.pdf")
            plt.close()


plot_fields()
