def CenterOfMass(Array, Height, Width):
    i = 0
    j = 0
    RowWeight = 0.0
    ColumnWeight = 0.0
    TotalWeight = 0.0
    while i < Height:
        while j < Width:
            if Array[i][j] == True:
                RowWeight += i
                ColumnWeight += j
                TotalWeight += 1
            j +=1
        j = 0
        i += 1
    RowWeight /= TotalWeight
    ColumnWeight /= TotalWeight
    return [RowWeight,ColumnWeight]

def Toggle(a):
    if a == True:
        return False
    if a == False:
        return True


def Bank(Array,ColumnRange):
    ## bank left
    moves = []
    H = len(Array)
    pos = []
    atoms = 0
    for j in range(ColumnRange[0]):
        for i in range(H):
            if Array[i][j] == True:
                pos.append([i,j])

    path = []
    i = 0
    j = 0
    invert = True
    while i < ColumnRange[0]:
        while j < H and j>-1:
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

    i = 0
    j = 0
    while i < len(pos):
        if Array[path[j][0]][path[j][1]] == True:
           Array = Move(Array,path[j],path[i])
           moves.append([path[j],path[i]])
           i += 1
        j += 1
    return moves

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
        moves.append([pos[k],j])
        k += 1
        j += 1
        extras -= 1
    while j<Range[0]:
        Row[j] = False
        j += 1
    while j <= Range[1]:
        Row[j] = True
        moves.append([pos[k],j])
        k += 1
        j += 1
    while j < dim - extras:
        Row[j] = False
        j += 1
    while j < dim:
        Row[j] = True
        moves.append([pos[k],j])
        k += 1
        j += 1
    
    return [Row,moves]

def Move(Array,position1,position2):
    NewArray = Array
    NewArray[position1[0]][position1[1]] = False
    NewArray[position2[0]][position2[1]] = True
    return NewArray

def Balance(Array,Range,COM,Height,Width,RowTotal):
    SufficientAtoms = Height
    Moves = []

    ## center is always the highest row of the lower half of the range
    center = 0
    if (Range[1] - Range[0])%2 == 0:
        center = (Range[1] - Range[0])/2 + Range[0]
    if (Range[1] - Range[0])%2 == 1:
        center = (Range[1] - Range[0] - 1)/2 + Range[0]

    SufficientAtomsLower = (center - Range[0] + 1)*SufficientAtoms
    SufficientAtomsUpper = (Range[1] - center)*SufficientAtoms
    
    i = Range[0]
    j = 0
    Lower = 0
    Upper = 0
    while i<=center:
        Lower += RowTotal[i]
        i += 1
    i = center + 1
    while i <= Range[1]:
        Upper += RowTotal[i]
        i += 1
    
    while SufficientAtomsLower>Lower or SufficientAtomsUpper>Upper:
        
        i = center + 1 
        moveto = []
        movefrom = []
        j = 0
        if SufficientAtomsLower<Lower:
            if SufficientAtomsUpper>Upper:
                while j < Width:
                    while i <= Range[1]:
                        if Array[i][j] == False:
                            moveto = [i,j]
                            k = center
                            while k >= Range[0]:
                                if Array[k][j] == True:
                                    movefrom = [k,j]
                                    k = Range[0] - 1   
                                k -= 1
                        if len(moveto) == 0 or len(movefrom) == 0:
                            i += 1
                        else:
                            break
                    if len(moveto) == 0 or len(movefrom) == 0:
                        i = center + 1
                        j += 1
                    else:
                        Upper += 1
                        break

            

        i = center
        j = 0
        if SufficientAtomsLower>Lower:
            if SufficientAtomsUpper<Upper:
                while j < Width:
                    while i >= Range[0]:
                        if Array[i][j] == False:
                            moveto = [i,j]
                            k = center + 1
                            while k <= Range[1]:
                                if Array[k][j] == True:
                                    movefrom = [k,j]
                                    k = Range[1] + 1   
                                k += 1
                        if len(moveto) == 0 or len(movefrom) == 0:
                            i -= 1
                        else:
                            break
                    if len(moveto) == 0 or len(movefrom) == 0:
                        i = center
                        j += 1
                    else:
                        Lower += 1
                        break
                        
        
        if len(moveto) == 0 or len(movefrom) == 0:
            Array = Array
        else:
            Array = Move(Array,movefrom,moveto)
            RowTotal[movefrom[0]] -= 1
            RowTotal[moveto[0]] += 1
            Moves.append([movefrom,moveto])
                
                
    Range1 = [center + 1,Range[1]]
    Range0 = [Range[0],center]
    NewArray = Array
    return [NewArray,RowTotal,Range0,Range1,Moves] 
    
def RowSum(Array,Height,Width):
    RowTotal = []
    Dummy = 0
    i = 0
    j = 0
    while i < Height:
        while j < Width:
            if Array[i][j] == True:
                Dummy += 1
            j += 1
        RowTotal.append(Dummy)
        Dummy = 0
        j = 0
        i += 1

    return RowTotal

def Transpose(Array, Height, Width):
    i = 0
    j = 0
    NewArray = []
    Row = []
    while i < Height:
        while j < Width:
            Row.append(Array[j][i])
            j += 1
        j = 0
        NewArray.append(Row)
        Row = []
        i += 1
    return NewArray

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

def BalanceCompress2(Array):
    Height = len(Array)
    Width = len(Array[0])
    ## balancw&compress alg:
    import datetime


        #start clock
    Start = datetime.datetime.now()
    Moves = []

        #find COM
    COM = CenterOfMass(Array,Height,Width)

            #make row totals
    RowTotals = RowSum(Array,Height,Width)

    RowRange = [[0,Height - 1]]
    x = float(Height)
    x -= 1
    x /= 2
    ColumnRange = [int(round(COM[1] - x))]
    ColumnRange.append(ColumnRange[0] + Height - 1)

    if ColumnRange[0] < 0:
        ColumnRange[0] = 0
        ColumnRange[1] = Height - 1


    if ColumnRange[1] >= Width:
        ColumnRange[1] = Width - 1
        ColumnRange[0] = Width - Height


            #balance the rows
    BalancedRows = []
    i = 0
    while 1:
        PlaceHolder = Balance(Array,RowRange[i],COM,Height,Width,RowTotals)
        Array = PlaceHolder[0]
        RowTotals = PlaceHolder[1]
        if PlaceHolder[2][1] - PlaceHolder[2][0] != 0:
            RowRange.append(PlaceHolder[2])
        else:
            BalancedRows.append(PlaceHolder[2][1])
        if PlaceHolder[3][1] - PlaceHolder[3][0] != 0:
            RowRange.append(PlaceHolder[3])
        else:
            BalancedRows.append(PlaceHolder[3][1])
        i += 1
        if len(PlaceHolder[4]) != 0:
            Moves.append(PlaceHolder[4][0])
        if len(BalancedRows) == RowRange[0][1] - RowRange[0][0] + 1:
            break
    rowmoves = []
            #compress rows in range
    i = RowRange[0][0]
    while i <= RowRange[0][1]:
        placeholder = CompressRow(Array[i],ColumnRange,Height)
        Array[i] = placeholder[0]
        rowmoves.append(placeholder[1])
        Moves.append('row%d' %i)
        i += 1

    newmoves = Bank(Array,ColumnRange)
    i = 0
    for i in range(len(newmoves)):
        Moves.append(newmoves[i])

            #Stop Clock Here
    End = datetime.datetime.now()
    Duration = End - Start
    check = Check(Array,RowRange[0],ColumnRange)
    if check == True:
        fidelity = 1
    else:
        fidelity = 0


    return [Duration,Moves,fidelity,rowmoves]

