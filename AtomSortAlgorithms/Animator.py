import BalanceCompressAlgorithm
import matplotlib.pyplot as plt
from matplotlib import animation
import MakeBoolArray

def Animator(Array,moves,dim):
    StaticArray = Array
    ArrayDim = dim
    COM = BalanceCompressAlgorithm.CenterOfMass(StaticArray, ArrayDim)
    def MakeMove(Array,move,COM):
        dim = len(Array)
        i = 0
        if 1 > 0:
            if len(move) == 2:
                Array = BalanceCompressAlgorithm.Move(Array,move[0],move[1])
            else:
                if move[0] == 'r':
                    row = int(move[3])
                    RowTotals = BalanceCompressAlgorithm.RowSum(Array,ArrayDim)
                    Array[int(move[3])] = BalanceCompressAlgorithm.CompressRow(COM[1],ArrayDim, RowTotals[int(move[3])])
                if move[0] == 'c':
                    col = int(move[3])
                    Array = BalanceCompressAlgorithm.Transpose(Array,dim)
                    Array = BalanceCompressAlgorithm.CompressRow(Array,col,COM)
                    Array = BalanceCompressAlgorithm.Transpose(Array,dim)
            i += 1
        return Array

    def update(n):
        if n > 0 and n < len(moves):
            ax.cla()
            plotArray(Array,n-1)

    def plotArray(Array,n):
        Array = MakeMove(Array,moves[n],COM)
        i = 0
        j = 0
        x = []
        y = []
        while i<10:
            while j<10:
                if Array[i][j] == True:
                    x.append(j)
                    y.append(i)
                j += 1
            i += 1
            j = 0
        ax.plot(x,y,'ro')
        plt.grid(True)
        plt.autoscale(False)
        plt.ylim(-1,10)
        plt.xlim(-1,10)



    fig = plt.figure(figsize=(10,10))
    ax = fig.add_subplot(1,1,1)
    anim  = animation.FuncAnimation(fig, update,interval = 100)
    plt.show()
