def MakeRectArray(ArraySites,LoadProbability):
    import random
    [Width, Height] = ArrayDimensions(ArraySites, LoadProbability)
    Array = []
    Row = []
    i = 0
    j = 0
    while i < Height:
        while j < Width:
            b = random.random()
            if b < LoadProbability:
                Row.append(True)
            else:
                Row.append(False)
            j += 1
        j = 0
        i += 1
        Array.append(Row)
        Row = []
    return Array

def ArrayDimensions(Num, Prob):
    import math
    Height = int(math.sqrt(Prob*Num))
    Width = int(Num/Height)
    return [Width, Height]
