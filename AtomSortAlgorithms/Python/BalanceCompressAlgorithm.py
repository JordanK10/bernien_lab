def CenterOfMass(Array, ArrayDimension):
    i = 0
    j = 0
    RowWeight = 0.0
    ColumnWeight = 0.0
    TotalWeight = 0.0
    while i < ArrayDimension:
        while j < ArrayDimension:
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

def CompressRow(center,Dim,RowTotal):
    i = 0
    a = float(RowTotal)
    NewRow = []
    Start = center - a/2
    End  = center + a/2 
    Start = int(round(Start))
    End = int(round(End)) - 1
    if Start<0:
        Start = 0
        End = int(a) - 1
    if End>Dim-1:
        End = Dim - 1
        Start = Dim - a
    i = 0
    while i < Start:
        NewRow.append(False)
        i +=1
    while i <= End:
        NewRow.append(True)
        i += 1
    while i <Dim:
        NewRow.append(False)
        i += 1
    return NewRow

def Move(Array,position1,position2):
    NewArray = Array
    NewArray[position1[0]][position1[1]] = False
    NewArray[position2[0]][position2[1]] = True
    return NewArray

def Balance(Array,Range,COM,Dim,SufficientAtoms,RowTotal):

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
                while j < Dim:
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
                while j < Dim:
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
    
def RowSum(Array,Dim):
    RowTotal = []
    Dummy = 0
    i = 0
    j = 0
    while i < Dim:
        while j < Dim:
            if Array[i][j] == True:
                Dummy += 1
            j += 1
        RowTotal.append(Dummy)
        Dummy = 0
        j = 0
        i += 1

    return RowTotal

def Transpose(Array, Dim):
    i = 0
    j = 0
    NewArray = []
    Row = []
    while i < Dim:
        while j < Dim:
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

def BalanceCompress(Array,ArrayDim,TargetDim):

    ## balancw&compress alg:
    import datetime


        #start clock
    Start = datetime.datetime.now()
    Moves = []

        #find COM
    COM = CenterOfMass(Array,ArrayDim)

            #make row totals
    Dummy = 0
    RowTotals = RowSum(Array,ArrayDim)
    
    atoms = 0
    if TargetDim == 'max':
        i = 0
        while i < ArrayDim:
            atoms += RowTotals[i]
            i += 1
        TargetDim = int(atoms/ArrayDim)
        RowRange = [[0,ArrayDim - 1]]

        x = float(TargetDim)
        x /= 2

        ColumnRange = [COM[1] - x,COM[1] + x - 1]
        ColumnRange[0] = round(ColumnRange[0])
        ColumnRange[1] = round(ColumnRange[1])

        ColumnRange[0] = int(ColumnRange[0])
        ColumnRange[1] = int(ColumnRange[1])

        if ColumnRange[0] < 0:
            ColumnRange[0] = 0
            ColumnRange[1] = TargetDim - 1


        if ColumnRange[1] >= ArrayDim:
            ColumnRange[1] = ArrayDim - 1
            ColumnRange[0] = ArrayDim - TargetDim
        

    else:
        x = float(TargetDim)
        x /= 2
            #Use COM to create a range of the rows for the final assortment
        RowRange = [COM[0] - x,COM[0] + x]
        RowRange[0] = round(RowRange[0])
        RowRange[1] = round(RowRange[1])

        RowRange[0] = int(RowRange[0])
        RowRange[1] = int(RowRange[1])

        if RowRange[0] < 0:
            RowRange[0] = 0
            RowRange[1] = TargetDim - 1


        if RowRange[1] >= ArrayDim:
            RowRange[1] = ArrayDim - 1
            RowRange[0] = ArrayDim - TargetDim

        RowRange = [RowRange]

            #Use COM to create a Column range

        ColumnRange = [COM[1] - x,COM[1] + x - 1]
        ColumnRange[0] = round(ColumnRange[0])
        ColumnRange[1] = round(ColumnRange[1])

        ColumnRange[0] = int(ColumnRange[0])
        ColumnRange[1] = int(ColumnRange[1])

        if ColumnRange[0] < 0:
            ColumnRange[0] = 0
            ColumnRange[1] = TargetDim - 1


        if ColumnRange[1] >= ArrayDim:
            ColumnRange[1] = ArrayDim - 1
            ColumnRange[0] = ArrayDim - TargetDim
    
    i = RowRange[0][0]
    while i <= RowRange[0][1]:
        Dummy += RowTotals[i]
        i += 1
    
    if Dummy>=TargetDim**2:
        RowTotals = RowSum(Array,ArrayDim)

            #balance the rows
        BalancedRows = []
        i = 0
        while 1:
            PlaceHolder = Balance(Array,RowRange[i],COM,ArrayDim,TargetDim,RowTotals)
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

            #compress rows in range
        i = RowRange[0][0]
        while i <= RowRange[0][1]:
            Array[i] = CompressRow(COM[1],ArrayDim,RowTotals[i])
            Moves.append('row%d' %i)
            i += 1

            #Stop Clock Here
        End = datetime.datetime.now()
        Duration = End - Start
        check = Check(Array,RowRange[0],ColumnRange)
        if check == True:
            fidelity = 1
        else:
            fidelity = 0

        return [Duration,Moves,fidelity]


            #move all extra atoms into a bank (to do)

    else:
            #Compress COM Columns (not necessary, usually. maybe)

        Array = Transpose(Array,ArrayDim)
        i = ColumnRange[0]
        while i <= ColumnRange[1]:
            Array[i] = CompressRow(COM[0],ArrayDim,RowTotals[i])
            i += 1
            Moves.append("col%d" %i)
        Array = Transpose(Array,ArrayDim)

        Dummy = 0
        RowTotals = RowSum(Array,ArrayDim)
        i = RowRange[0][0]
        while i <= RowRange[0][1]:
            Dummy += RowTotals[i]
            i += 1
        if Dummy>=TargetDim**2:
            RowTotals = RowSum(Array,ArrayDim)


                #balance the rows
            BalancedRows = []
            i = 0
            while 1:
                PlaceHolder = Balance(Array,RowRange[i],COM,ArrayDim,TargetDim,RowTotals)
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
                if len(BalancedRows) == TargetDim:
                    break

                #compress rows in range
            i = RowRange[0][0]
            while i <= RowRange[0][1]:
                Array[i] = CompressRow(COM[1],ArrayDim,RowTotals[i])
                i += 1
                Moves.append('row%d'%i)
            

                #Stop Clock Here
            End = datetime.datetime.now()
            Duration = End - Start

            check = Check(Array,RowRange[0],ColumnRange)
            if check == True:
                fidelity = 1
            else:
                fidelity = 0

            return [Duration,Moves,fidelity]
        else:
            return [datetime.timedelta(0),[],0]

