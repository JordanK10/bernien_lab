def Toggle(a):
    if a == True:
        return False
    if a == False:
        return True

def makepath(ArrayDim,TargetDim):
    
    #standard start
    def StdStart(Path):
        i = diff
        j = 0
        invert = True
        while i < ArrayDim:
            while j < TargetDim and j>-1:
                path.append([j,i])
                if invert == True:
                    j += 1
                else:
                    j -= 1
            i += 1
            if invert == True:
                j -= 1
            else:
                j += 1
            invert = Toggle(invert)
        return Path

    #alternate start
    def AltStart(Path):
        i = ArrayDim - 1
        j = TargetDim - 1
        invert = False
        while i > diff - 1:
            while j < TargetDim and j>-1:
                path.append([j,i])
                if invert == True:
                    j += 1
                else:
                    j -= 1
            i -= 1
            if invert == True:
                j -= 1
            else:
                j += 1
            invert = Toggle(invert)
        return Path
        
    #down
    def Down(Path):
        i = ArrayDim - 1
        j = TargetDim
        invert = True
        while i > diff - 1:
            while j < ArrayDim and j > TargetDim - 1:
                path.append([j,i])
                if invert == True:
                    j += 1
                else:
                    j -= 1
            i -= 1
            if invert == True:
                j -= 1
            else:
                j += 1
            invert = Toggle(invert)
        return Path

    #left
    def Left(Path):
        i = ArrayDim - 1
        j = TargetDim
        invert = False
        while j < ArrayDim:
            while i < ArrayDim and i > diff - 1:
                path.append([j,i])
                if invert == True:
                    i += 1
                else:
                    i -= 1
            j += 1
            if invert == True:
                i -= 1
            else:
                i += 1
            invert = Toggle(invert)
        return Path

    #right
    def Right(Path):
        i = diff
        j = TargetDim
        invert = True
        while j < ArrayDim:
            while i < ArrayDim and i > diff - 1:
                path.append([j,i])
                if invert == True:
                    i += 1
                else:
                    i -= 1
            j += 1
            if invert == True:
                i -= 1
            else:
                i += 1
            invert = Toggle(invert)
        return Path
    

    #finish
    def Finish(Path):
        i = diff -1 
        j = ArrayDim - 1
        invert = False
        while i>-1:
            while j>-1 and j <ArrayDim:
                path.append([j,i])
                if invert == True:
                    j += 1
                else:
                    j -= 1
            i -= 1
            if invert == True:
                j -= 1
            else:
                j += 1
            invert = Toggle(invert)
        return Path

    diff = ArrayDim - TargetDim
    path = []

    if diff%2 == 1:
        path = StdStart(path)
        path = Left(path)
    else:
        if ArrayDim%2 == 0:
            path = AltStart(path)
            path = Right(path)
        if ArrayDim%2 == 1:
            path = StdStart(path)
            path = Down(path)
    path = Finish(path)
    return path

def Move(Array,position1,position2):
    NewArray = Array
    NewArray[position1[0]][position1[1]] = False
    NewArray[position2[0]][position2[1]] = True
    return NewArray

def Advance(Array,i,path,moves):
    while i>0:
        if Array[path[i-1][0]][path[i-1][1]] == True:
            break
        Array = Move(Array,path[i],path[i-1])
        moves.append([path[i],path[i-1]])
        i -= 1
    return [Array,moves]

def Check(Array,RowRange,ColumnRange):
    i = RowRange[0]
    j = ColumnRange[0]
    Check = True
    while i <= RowRange[1]:
        while j <= ColumnRange[1]:
            if Array[i][j] == False:
                Check = False
            j += 1
        i += 1
        j = ColumnRange[0]

    return Check


def snake(Array,ArrayDim,TargetDim):
    import datetime



    if TargetDim == 'max':
        import math
        i = 0
        j = 0
        atoms = 0
        while i < ArrayDim:
            while j < ArrayDim:
                if Array[i][j] == True:
                    atoms += 1
                j += 1
            i += 1
            j = 0
        TargetDim = math.sqrt(atoms)
        TargetDim = int(TargetDim)
    path = makepath(ArrayDim,TargetDim)

    start = datetime.datetime.now()
    length = len(path)
    i = 0
    j = 0
    moves = []
    while i < length:
        if Array[path[i][0]][path[i][1]] == True:
            placeholder = Advance(Array,i,path,moves)
            Array = placeholder[0]
            moves = placeholder[1]
        i += 1

    end  = datetime.datetime.now()
    duration = end - start

    RowRange = [0,TargetDim - 1]
    ColumnRange = [ArrayDim - TargetDim,ArrayDim - 1]
    check = Check(Array,RowRange,ColumnRange)
    if check == True:
        fidelity = 1
    else:
        fidelity = 0
    return[duration,moves,fidelity]
    
        
        
