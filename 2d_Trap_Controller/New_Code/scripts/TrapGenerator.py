import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from numpy.random import uniform as ran
from IPython.display import clear_output, display

center_x = 90*1e6
center_y = 90*1e6
 ###
 ### Parameters
 ###
trap_x_num = input("number of traps in x")
trap_y_num = input("number of traps in y")

pi2 = 2*np.pi
optphase = True

txmin = center_x-(trap_x_num*1e6)/2
txmax = center_x+(trap_x_num*1e6)/2
tymin = center_y-(trap_y_num*1e6)/2
tymax = center_y+(trap_y_num*1e6)/2

xpow = float(1/float(trap_x_num))
ypow = float(1/float(trap_x_num))

xfrequencies = np.linspace(txmin,txmax,trap_x_num+1)
yfrequencies = np.linspace(tymin,tymax,trap_y_num+1)
###
### Wavetable
###

fidelity = 1e6
wf = 1e3
sampleRate=1.024e9
timestep = 1.0/sampleRate
tableLength = int(sampleRate/wf)
duration = 0.00001
numSamples = int(duration * sampleRate)

x = np.linspace(0,numSamples,numSamples)

tableInd = np.linspace(0,tableLength,num=tableLength+1)
coeff = pi2/tableLength
waveTable = [np.exp((s*coeff)*1j) for s in tableInd]

"""
10 trap phases:
Freq(MHz)   phase(fractional period)
85          0.3899609375
86          0.405020507812
87          0.884223632813
88          0.846274414063
89          0.60730859375
90          0.666987304688
91          0.87426953125
92          0.520982421875
93          0.001861328125
94          0.519811523438

max peak = 3.65838438877
max peak/trap = .365838438877 
"""


def genWave(freq,phase):
    pha = int(phase*tableLength)
    wave = []
    for s in range(numSamples):
        if (pha > tableLength):
            pha -= int(tableLength)
        if pha < 0:
            pha += int(tableLength)
        wave.append(waveTable[pha])
        pha += int(freq/wf)
    return wave,phase

'''
Minimization
'''
def findMin(lb_guess,ub_guess,fid,wv,freq):
    tempwaves = []
    maxima = []

    for i in range(lb_guess,ub_guess,int(tableLength/fid)):
        currTempWave = genWave(freq,i/tableLength)
        tempwaves.append(np.add(currTempWave[0],wv))
        maxima.append(np.amax(tempwaves[-1]))
#         plt.plot(x,tempwaves[-1])
#         plt.show()
    pind = np.argmin(maxima)
    wv = tempwaves[pind]
#     print("Frequency: ", w,"Minimized at index : ",pind)

    return wv,(lb_guess+pind*tableLength/fid)



###
### Optimal Phase Generation
###
'''
Minimization
'''

best_guess_wave = []
initial_guess = 0
num_traps=[]
peaks = []
def genPhases(tmin,tmax,num_traps):
    wave = [(genWave(tmin,0))[0]]
    phases = [0.]
    frequencies = np.linspace(tmin,tmax,num_traps+1)
    print(frequencies,tmin)
    for w in frequencies[1:]:
            lb_guess = initial_guess #left bound of min phase
            ub_guess = tableLength #right bound of min phase
            #Save first cosine wave
            for fidelity in [1e1,1e2,1e3,1e4,1e5,1e6]:

                data = findMin(lb_guess,ub_guess,fidelity,wave,w)
                lb_guess = int(data[1]-(tableLength/fidelity)/2)
                if lb_guess < 0:
                    lb_guess = 0
                ub_guess = int(data[1]+(tableLength/fidelity)/2)
                if ub_guess > tableLength:
                    ub_guess = tableLength
                best_guess_wave = data[0]
#                     print("FINAL: Fid: ",fidelity, "lower: ",lb_guess,"upper: ", ub_guess)

                if fidelity == 1e6:

                    wave=best_guess_wave
                    phases.append(data[1])
#                         print("FREQUENCY: ", w,"PHASE: ",phases[-1])
            wave = best_guess_wave
    print(phases)
    return(frequencies,phases)

end = " "

if optphase:
    end = "to.txt"
else:
    end = "t.txt"

fname = str(trap_x_num)+"x"+str(trap_y_num)+end
f=open("../bin/DefaultTrapConfigurations/"+fname,"w")
gname = str(trap_x_num)
g=open("../bin/DefaultTrapConfigurations/"+gname+"X"+end,"w")
f.write(str(trap_x_num)+" "+str(trap_x_num)+"\n\n")
xfrequencies,xphases = genPhases(txmin,txmax,trap_x_num)
for freq,phase in zip(xfrequencies,xphases):
    f.write(str(float(freq)) + " " + str(xpow) + " " + str(phase)  +"\n")
    g.write(str(float(freq)) + " " + str(xpow) + " " + str(phase)  +"\n")
f.write("\n")
g.close()
g=open("../bin/DefaultTrapConfigurations/"+gname+"Y"+end,"w")

yfrequencies,yphases = genPhases(tymin,tymax,trap_y_num)

for freq,phase in zip(yfrequencies,yphases):
    f.write(str(float(freq)) + " " + str(ypow) + " " + str(phase) +"\n")
    g.write(str(float(freq)) + " " + str(xpow) + " " + str(phase)  +"\n")

f.close()
g.close()
