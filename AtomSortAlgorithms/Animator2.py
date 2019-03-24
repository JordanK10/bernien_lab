import RectangularBC
import matplotlib.pyplot as plt
from matplotlib import animation
import MakeBoolArray

def CompressRow(Row,ColRange,suff):
    dim = len(Row)
    x = float(suff)
    x /= 2
    Range = ColRange

    if Range[0] < 0:
        Range[0] = 0
        Range[1] = suff - 1


    if Range[1] >= dim:
        Range[1] = dim - 1
        Range[0] = dim - suff

    atoms = 0
    pos = []
    for i in range(dim):
        if Row[i] == True:
            atoms += 1
            pos.append(i)
    diff = atoms - suff
    if diff<0:
        diff = 0

    j = 0
    moves = []
    extras = diff
    k = 0
    while j < diff and j < Range[0]:
        Row[j] = True
        k += 1
        j += 1
        extras -= 1
    while j<Range[0]:
        Row[j] = False
        j += 1
    while j <= Range[1]:
        Row[j] = True
        k += 1
        j += 1
    while j < dim - extras:
        Row[j] = False
        j += 1
    while j < dim:
        Row[j] = True
        k += 1
        j += 1
    
    return Row

def Animator2(Array,moves):
    StaticArray = Array
    Height = len(Array)
    Width = len(Array[0])
    COM = RectangularBC.CenterOfMass(StaticArray, Height, Width)
    x = float(Height)
    x -= 1
    x /= 2
    ColumnRange = [int(round(COM[1] - x))]
    ColumnRange.append(ColumnRange[0] + Height - 1)
    def MakeMove(Array,move,COM):
        dim = len(Array)
        Height = len(Array)
        i = 0
        if 1 > 0:
            if len(move) == 2:
                Array = RectangularBC.Move(Array,move[0],move[1])
            else:
                row = int(move[3])
                Array[row] = CompressRow(Array[row],ColumnRange, Height)
            i += 1
        return Array

    def update(n):
        if n > 0 and n < len(moves):
            ax.cla()
            plotArray(Array,n-1)

    def plotArray(Array,n):
        Height = len(Array)
        Width = len(Array[0])
        Array = MakeMove(Array,moves[n],COM)
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
        plt.grid(True)
        plt.autoscale(False)
        plt.ylim(-1,Height)
        plt.xlim(-1,Width)



    fig = plt.figure(figsize=(10,10))
    ax = fig.add_subplot(1,1,1)
    anim  = animation.FuncAnimation(fig, update,interval = 100)
    plt.show()
