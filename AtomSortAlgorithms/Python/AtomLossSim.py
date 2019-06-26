import RectangularBC
import matplotlib.pyplot as plt
from matplotlib import animation
from MakeRectArray import MakeRectArray
from Move import Move
import random
import math
import time

def FindVacancies(Array,Range):
    H = len(Array)
    vacant = []
    for i in range(H):
        j = Range[0]
        while j<=Range[1]:
            if Array[i][j] == False:
                vacant.append([i,j])
            j += 1
    return vacant

def FillVacancies(Array, Vacant, Bank, Range):
    H = len(Array)
    W = len(Array[0])
    B = len(Bank)
    V = len(Vacant)
    done = False

    for i in range(V):
        row = Vacant[i][0]
        for j in range(len(Bank)):
            if Bank[j][0] == row:
                Array[row] = CompressRow(Array[row],Range,H)
                del Bank[j]
                done = True
                break

        if done == False:
            k = row
            while k < H:
                for l in range(B):
                    if Bank[l][0] == k:
                        Array = Move(Array,Bank[l],[row,Bank[l][1]])
                        del Bank[l]
                        done = True
                        break
                if done == True:
                    break
                else:
                    k += 1
            Array[row] = CompressRow(Array[row],Range,H)

        if done == False:
            k = row
            while k >= 0:
                for l in range(B):
                    if Bank[l][0] == k:
                        Array = Move(Array,Bank[l],[row,Bank[l][1]])
                        del Bank[l]
                        done = True
                        break
                if done == True:
                    break
                else:
                    k -= 1
            Array[row] = CompressRow(Array[row],Range,H)
    return [Array,Bank]
                  
def AtomLoss(duration,lifetime):
    if lifetime<duration:
        return False
    else:
        return True
    
def ArrayDecay(Array, time, LossTime):
    n = len(Array)
    m = len(Array[0])
    for i in range(n):
        for j in range(m):
            if Array[i][j] == True:
                Array[i][j] = AtomLoss(time,LossTime[i][j])
    return Array

def MakeLossTime(n,m,lifetime):
    import math
    import random
    LossTime=[]
    row = []
    for i in range(n):
        for j in range(m):
            r = random.random()
            losstime = -lifetime*math.log(1-r)
            row.append(losstime)
        LossTime.append(row)
        row = []
    return LossTime

def CompressRow(Row,Range,suff):
    dim = len(Row)
    NewRow = []
    atoms = 0
    for i in range(dim):
        if Row[i] == True:
            atoms += 1
    diff = atoms - suff
    extras = 0
    missing = 0
    if diff<0:
        missing = -diff
    else:
        extras = diff

    j = 0
    while j < extras and j < Range[0]:
        NewRow.append(True)
        j += 1
        extras -= 1
    while j<(Range[0] + missing):
        NewRow.append(False)
        j += 1
    while j <= Range[1]:
        NewRow.append(True)
        j += 1
    while j < dim - extras:
        NewRow.append(False)
        j += 1
    while j < dim:
        NewRow.append(True)
        j += 1
    return NewRow

def Animator2(Array,Bank,start):
    camera_freq = 100
    lifetime = 100

    LossTime = MakeLossTime(len(Array),len(Array[0]),lifetime)

    sleeptime = 1/camera_freq
    from datetime import datetime
    def update(n):
        if n > 0 and len(Bank)>0:
            ax.cla()
            if n%2 == 0:
                plotArray1(Array,n)
            else:
                plotArray2(Array,n,Bank,LossTime,lifetime)

    def plotArray1(Array,n):
        Range = [4,10]
        Height = len(Array)
        Width = len(Array[0])
        current = datetime.now()
        duration = current - start
        timediff = duration.seconds + duration.microseconds * 10**(-6)
        timediff = timediff
        Array = ArrayDecay(Array,timediff,LossTime)
        i = 0
        j = 0
        x = []
        y = []
        while i<Height:
            while j<Width:
                if Array[i][j] == True:
                    x.append(j)
                    y.append(i)
                j += 1
            i += 1
            j = 0
        ax.plot(x,y,'ro')
        ax.set_title(str(timediff) + " s")
        plt.grid(True)
        plt.autoscale(False)
        plt.ylim(-1,Height)
        plt.xlim(-1,Width)
        time.sleep(sleeptime)


    def plotArray2(Array,n,Bank,LossTime,lifetime):
        Range = [4,10]
        Height = len(Array)
        Width = len(Array[0])
        Vacant = FindVacancies(Array,Range)
        [Array,Bank] = FillVacancies(Array,Vacant, Bank, Range)
        for i in range(len(Vacant)):
            LossTime[Vacant[i][0]][Vacant[i][1]] = lifetime
        i = 0
        j = 0
        x = []
        y = []
        while i<Height:
            while j<Width:
                if Array[i][j] == True:
                    x.append(j)
                    y.append(i)
                j += 1
            i += 1
            j = 0
        ax.plot(x,y,'ro')
        current = datetime.now()
        duration = current - start
        timediff = duration.seconds + duration.microseconds * 10**(-6)
        timediff = timediff
        ax.set_title(str(timediff) + " s")
        plt.grid(True)
        plt.autoscale(False)
        plt.ylim(-1,Height)
        plt.xlim(-1,Width)
        time.sleep(.01)



    fig = plt.figure(figsize=(10,10))
    ax = fig.add_subplot(1,1,1)
    anim  = animation.FuncAnimation(fig, update,interval = 10)
    plt.show()

from datetime import datetime
Bank = [[0,0],[1,0],[2,0],[3,0],[4,0],[5,0],[6,0]]
Array = [[True,False,False,False,True,True,True,True,True,True,True,False,False,False],[True,False,False,False,True,True,True,True,True,True,True,False,False,False],[True,False,False,False,True,True,True,True,True,True,True,False,False,False],[True,False,False,False,True,True,True,True,True,True,True,False,False,False],[True,False,False,False,True,True,True,True,True,True,True,False,False,False],[True,False,False,False,True,True,True,True,True,True,True,False,False,False],[True,False,False,False,True,True,True,True,True,True,True,False,False,False]]
start = datetime.now()
Bank1 = [[0,0],[1,0],[2,0],[3,0],[4,0],[5,0],[6,0],[0,1],[1,1],[2,1],[3,1],[4,1],[5,1],[6,1]]
Array2 = [[True,True,False,False,True,True,True,True,True,True,True,False,False,False],[True,True,False,False,True,True,True,True,True,True,True,False,False,False],[True,True,False,False,True,True,True,True,True,True,True,False,False,False],[True,True,False,False,True,True,True,True,True,True,True,False,False,False],[True,True,False,False,True,True,True,True,True,True,True,False,False,False],[True,True,False,False,True,True,True,True,True,True,True,False,False,False],[True,True,False,False,True,True,True,True,True,True,True,False,False,False]]
Animator2(Array,Bank,start)
