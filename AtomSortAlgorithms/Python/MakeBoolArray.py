import random
def MakeBoolArray(ArrayDimension, LoadProbability):
    Array = []
    Row = []
    i = 0
    j = 0
    while i < ArrayDimension:
        while j < ArrayDimension:
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

    
