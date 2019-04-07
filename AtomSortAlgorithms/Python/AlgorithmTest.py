import BalanceCompressAlgorithm
import MakeRectArray
import RectangularBC
import Snake
import Hungarian
import MakeBoolArray
import datetime
import xlsxwriter
import matplotlib
from Animator import Animator
from Animator2 import Animator2

# these are the 'settings'

title = "RectangularBC"
ArrayDim = 10
TargetDim = 'max'
trials = 1
LoadProbability = .6
algorithm = 3
RecordData = False
MakeAnimation = True
Sites = 100

# Algorithm number's:
# 1 -> Balance&Compress
# 2 -> Hungarian
# 3 -> snake
# 4 -> RectangularBC





time = []
fidelity = []
moves = []
StaticArray = []
DummyRow = []
DummyVar = True

i = 0
while i < trials:
    Array = MakeBoolArray.MakeBoolArray(ArrayDim,LoadProbability)
    Array2 = MakeRectArray.MakeRectArray(Sites,LoadProbability)
    j = 0
    k = 0
    while j < ArrayDim:
        while k < ArrayDim:
            DummyVar = Array[j][k]
            DummyRow.append(DummyVar)
            k += 1
        StaticArray.append(DummyRow)
        DummyRow = []
        k = 0
        j += 1


    if algorithm == 1:
        placeholder = BalanceCompressAlgorithm.BalanceCompress(Array, ArrayDim,TargetDim)
    if algorithm == 2:
        placeholder = Hungarian.Hungarian(Array,ArrayDim,TargetDim)
    if algorithm == 3:
        placeholder = Snake.snake(Array,ArrayDim,TargetDim)
    if algorithm == 4:
        StaticArray = []
        H = len(Array2)
        W = len(Array2[0])
        j = 0
        k = 0
        while j < H:
            while k < W:
                DummyVar = Array2[j][k]
                DummyRow.append(DummyVar)
                k += 1
            StaticArray.append(DummyRow)
            DummyRow = []
            k = 0
            j += 1
        placeholder = RectangularBC.BalanceCompress2(Array2)
        rowmoves = placeholder[3]
       
    time.append(placeholder[0].microseconds + placeholder[0].seconds*(10**6))
    moves.append(placeholder[1])
    fidelity.append(placeholder[2])

    i += 1

timetotal = 0
movetotal = 0
for i in range(len(time)):
    timetotal += time[i]
    movetotal += len(moves[i])

timeavg = float(timetotal/trials) 
moveavg = float(movetotal/trials)

if RecordData == True:
    i = 0
    j = 0
    workbook = xlsxwriter.Workbook(title)
    worksheet = workbook.add_worksheet()
    while i<trials:
        worksheet.write(0,3*i,"Trial:")
        worksheet.write(0,3*i + 1,i)
        worksheet.write(1,3*i,"Time:")
        worksheet.write(1,3*i + 1,time[i])
        worksheet.write(2,3*i,"Fidelity:")
        worksheet.write(2,3*i + 1,fidelity[i])
        worksheet.write(3,3*i, "Moves:")
        worksheet.write(3,3*i + 1,len(moves[i]))
        while j < len(moves[i]):
            if len(moves[i][j]) == 2:
                worksheet.write(4+j,3*i,str(moves[i][j][0]))
                worksheet.write(4+j,3*i + 1,str(moves[i][j][1]))
            else:
                worksheet.write(4+j,3*i,moves[i][j])
            j += 1
        i += 1
        j = 0
    workbook.close()

if MakeAnimation == True and algorithm == 4:
    moves = moves[0]
    k = len(moves)
    moves.append(moves[k-1])
    Animator2(StaticArray,moves)

if MakeAnimation == True and algorithm != 4:
    moves = moves[0]
    k = len(moves)
    moves.append(moves[k-1])
    Animator(StaticArray,moves,ArrayDim)


