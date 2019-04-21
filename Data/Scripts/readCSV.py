import csv
import numpy as np
from matplotlib import pyplot as plt
from scipy.signal import argrelextrema as extrema
import os.path
import numpy_indexed as npi

num_traps = 5
filetype = input("Spec or Scope? ")
scope_path = '../Spectrum_Data/Grainy_Scope/'
spec_path = '../Spectrum_Data/5_Trap_Spectrum/'
spec_units = 1000000.0
spec_yunits = 1000
scope_units = 1.0
units = 1
xcol = 0
ycol = 0
xunits = 0
yunits = 0
units = 1.0
watt = True
path = ' '
file_array = []

def assignGlobalVariables():

    global xcol; global ycol; global xunits; global yunits;
    global path; global units;

    if filetype == "spec":

        xcol = 0; ycol = 2
        xunits = "MHz";
        if watt:
            yunits = "mW"
        else:
            yunits = "dBm"
            spec_yunits=1

        path = spec_path
        units = spec_units

    elif filetype == "scope":

        xcol = 0; ycol = 1
        xunit = "Steps"; yunit = "Volts"
        path = scope_path
        units = scope_units


def plotData(files,plots,peaks):

    f = open("peaks.txt", "a")

    for (plot,file,extrema) in zip(plots,files,peaks):

        f.write("Plot title: " + file)

        fig = plt.figure()
        ax = fig.add_subplot(111)
        if filetype == "spec":
            if watt:
                plot[1] = [spec_yunits*10**((point-30)/10) for point in plot[1]]
            else:
                ax.scatter(extrema[0],extrema[1],color="red")
                for (x,y) in zip(extrema[0],extrema[1]):
                    f.write(str(y) + "dBm at " + str(x) + "MHz\n" )

        ax.plot(plot[0],plot[1],color="blue")
        plt.xlabel(xunits)
        plt.ylabel(yunits)
        plt.title(file)

        plt.show()



    return

def findPeaks(data_array):

    xvalues = data_array[0]
    yvalues = data_array[1]
    indices = extrema(np.array(yvalues),np.greater)[0]
    ext = [[],[]]
    temp = []

    for index in indices:
        temp.append(yvalues[index])

    indices = [index for tem, index in sorted(zip(temp, indices),reverse=True)]

    for (index,i) in zip(indices,range(len(indices))):
        if i < num_traps+5 or yvalues[index] > -10:
            ext[0].append(xvalues[index])
            ext[1].append(yvalues[index])


        else:
            break


    # for i in range(len(array)-1):
    #     if val >= array[i].temp and val <= array[i+1].temp:
    #         break
    #     ind_guess += 1
    # ind = ind_guess
    # val = array[ind].pres
    # for i in range(int(len(array)*.165)):
    #     if ind_guess + i < len(array):
    #         if val < array[ind_guess + i].pres:
    #             ind = ind_guess + i
    #             val = array[ind].pres
    #     if ind_guess - i > 0:
    #         if val < array[ind_guess - i].pres:
    #             ind = ind_guess - i
    #             val = array[ind].pres
    # print( "Peak found at " + str(val))
    return ext

def parseData(filename):

    data_array = [[],[]]
    peaks_array = []
    with open(path+filename, newline='') as csvfile:

        spamreader = csv.reader(csvfile, delimiter=',')
        i = 0
        for row in spamreader:

            if i<2:
                i+=1
                continue

            #Standard Data
            data_array[0].append(float(row[xcol])/units)
            data_array[1].append(float(row[ycol]))

        if filetype == "spec":
            peaks_array = findPeaks(data_array)
    return data_array,peaks_array

def grabFiles(path):

    for filename in os.listdir(path):
        if filename.endswith(".csv"):
            file_array.append(filename)

    return len(file_array)>0

if __name__ == "__main__":


    assignGlobalVariables()

    if not grabFiles(path):
        print("No files exist in this path...")
        exit()

    else:
        print("Files found... processing")

    plot_data = []
    peaks = []

    for file in file_array:
        temp = parseData(file)
        plot_data.append(temp[0])
        peaks.append(temp[1])

    plotData(file_array,plot_data,peaks)
