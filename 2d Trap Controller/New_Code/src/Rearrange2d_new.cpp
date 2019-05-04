#include Rearrange2d_new.h

using namespace std;

vector<bool> ColumnAt(vector<vector<bool>> Array, int dim){
  vector<bool> new_col(Array.size());
  for(k=0;k<Array.size();k++)
    new_col[k]=Array[k][dim];
  return new_col;
}

vector<bool> CompressRow(vector<bool> row, int left, int right, int atoms){
    int suff = right - left + 1;
    int diff = atoms - suff;
    int extras = diff;
    int dim = row.size();
    j = 0;
    vector<bool> newRow;
    while(j<diff && j<left){
        newRow.push_back(true);
        j ++;
        extras--;
    }
    while(j<left){
        newRow.push_back(false);
        j++;
    }
    while(j<=right){
        newRow.push_back(true);
        j++;
    }
    while(j<dim-extras){
        newRow.push_back(false);
        j++;
    }
    while(j<dim){
        newRow.push_back(true);
        j++;
    }
    return newRow;
}

vector<int> RowSum(vector<vector<bool>> Array, int dimension){
    vector<int> RowTotal(dimension+1);
    for(i=0; i < dimension; i++){
        for(j=0; j < dimension; j++){
            RowTotal[i+1] += int(Array[i][j]);
            RowTotal[0] += int(Array[i][j]);
        }
    }
    return RowTotal;
}

tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<RearrangementMove>> Balance(vector<vector<bool>> Array, vector<int> Range, int dim, int SufficientAtoms, vector<int> RowTotal){
    vector<RearrangementMove> moves;

    int center;
    if((Range[1] - Range[0])%2 == 0){
        center  = (Range[1] - Range[0])/2 + Range[0];
    }else{
        center  = (Range[1] - Range[0] - 1)/2 + Range[0];
    }
    int SuffLower = (center - Range[0] + 1)*SufficientAtoms;
    int SuffUpper = (Range[1] - center)*SufficientAtoms;


    i = Range[0];
    int Lower = 0;
    int Upper = 0;
    while(i <= center){
        Lower += RowTotal[i+1];
        i ++;
    }
    while(i <= Range[1]){
        Upper += RowTotal[i+1];
        i ++;
    }
    int counter = 0;

    while(SuffUpper > Upper || SuffLower > Lower){
        i = center + 1;
        vector<int> moveto = {};
        vector<int> movefrom = {};
        j = 0;
        if(SuffLower < Lower && SuffUpper < Upper){
            break;
            }
        if(SuffLower < Lower && SuffUpper > Upper){
            while(j < dim){
                while(i <= Range[1]){
                    if(Array[i][j] == false){
                        moveto.push_back(i);
                        moveto.push_back(j);
                        k = center;
                        while(k >= Range[0]){
                            if(Array[k][j] == true){
                                movefrom.push_back(k);
                                movefrom.push_back(j);
                                break;
                            }
                            k --;
                        }
                    }
                    if(moveto.size() == 0 || movefrom.size() == 0){
                        i ++;
                    }else{
                        break;
                    }
                }
                if(moveto.size() == 0 || movefrom.size() == 0){
                        i = center + 1;
                        j ++;
                        moveto = {};
                        movefrom = {};
                    }else{
                        Upper ++;
                        break;
                }
        }
    }
        i = center;
        j = 0;
        if(SuffLower > Lower && SuffUpper < Upper){
            while(j < dim){
                while(i >= Range[0]){
                    if(Array[i][j] == false){
                        moveto.push_back(i);
                        moveto.push_back(j);
                        k = center + 1;
                        while(k <= Range[1]){
                            if(Array[k][j] == true){
                                movefrom.push_back(k);
                                movefrom.push_back(j);
                                break;
                            }
                            k ++;
                        }
                    }
                    if(moveto.size() == 0 || movefrom.size() == 0){
                        i --;
                    }else{
                        break;
                    }
                }
                if(moveto.size() == 0 || movefrom.size() == 0){
                        i = center;
                        j ++;
                        moveto = {};
                        movefrom = {};
                    }else{
                        Lower ++;
                        break;
                }
        }
    }
    if(moveto.size() != 0 || movefrom.size() != 0){
    moves.push_back(RearrangementMove());
    moves[counter].dim = movefrom[1];
    moves[counter].row = false;
    moves[counter].startingConfig = ColumnAt(Array,movefrom[1]);
    Array[movefrom[0]][movefrom[1]] = false;
    Array[moveto[0]][moveto[1]] = true;
    moves[counter].endingConfig = ColumnAt(Array,movefrom[1]);
    counter ++;

    RowTotal[movefrom[0]+1] --;
    RowTotal[moveto[0]+1] ++;
    }
}
vector<int> Range1 = {center + 1,Range[1]};
vector<int> Range0 = {Range[0],center};
return make_tuple(Array,RowTotal,Range0,Range1,moves);
}

vector<vector<bool>> assignCol(vector<vector<bool>> Array, vector<bool> col, int index)
{
    for(i = 0;i<Array.size();i++){
        Array[i][index] = col[i];
    }
    return Array;
}

int* CenterOfMass(vector<vector<bool>> Array, int dim){
    float RowWeight = 0;
    float ColWeight = 0;
    float TotalWeight = 0;
    for(i=0; i < dim; i++){
        for(j=0; j < dim; j++){
            if (Array[i][j] == true){
                RowWeight += i;
                ColWeight += j;
                TotalWeight ++;
            }
        }
    }

    COM[0] = int(RowWeight /= TotalWeight);
    COM[1] = int(ColWeight /= TotalWeight);
    int *ptr = COM;
    return ptr;
}


vector<RearrangementMove> BalanceCompressAlg(vector<vector<bool>> Array, int mode){

  vector<RearrangementMove> initCols;
  vector<RearrangementMove> rows;
  vector<RearrangementMove> cols;
  vector<RearrangementMove> tempCols;


    int ArrayDim = Array.size();
    vector<int> RowTotals = RowSum(Array, ArrayDim);
    int atoms = RowTotals[0];
    int TargetDim = sqrt(atoms);
    int col_y;
    int col_z;
    int row_y;
    int row_z;

//default mode set center of mass as center of array
if(mode == 0){
    int *COM = CenterOfMass(Array, ArrayDim);

    float x = TargetDim/2;

        col_y = COM[1] - x;
        col_z = TargetDim + col_y - 1;

        if(col_y < 0){
            col_y = 0;
            col_z = TargetDim - 1;
        }
        if(col_z >= ArrayDim){
            col_z = ArrayDim - 1;
            col_y = ArrayDim - TargetDim;
        }

        row_y = COM[0] - x;
        row_z = TargetDim + row_y - 1;

        if(row_y < 0){
            row_y = 0;
            row_z = TargetDim - 1;
        }
        if(row_z >= ArrayDim){
            row_z = ArrayDim - 1;
            row_y = ArrayDim - TargetDim;
        }
}

//mode 5 = corner nearest the COM
if(mode == 5){
    int *COM = CenterOfMass(Array, ArrayDim);
    if(COM[0] < (ArrayDim - 1)/2){
        if(COM[1]<(ArrayDim - 1)/2){
            mode = 1;
        }else{
            mode = 2;
        }
    }else{
        if(COM[1]<(ArrayDim - 1)/2){
            mode = 3;
        }else{
            mode = 4;
        }
    }
}

//mode 1 = upper left corner
if(mode == 1){
    col_y = 0;
    col_z = TargetDim - 1;
    row_y = 0;
    row_z = TargetDim - 1;
}

//mode 2 = upper right corner
if(mode == 2){
    col_y = ArrayDim - TargetDim;
    col_z = ArrayDim - 1;
    row_y = 0;
    row_z = TargetDim - 1;
}

//mode 3 = lower left corner
if(mode == 3){
    col_y = 0;
    col_z = TargetDim - 1;
    row_y = ArrayDim - TargetDim;
    row_z = ArrayDim - 1;
}

//mode 4 = lower right corner
if(mode == 4){
    col_y = ArrayDim - TargetDim;
    col_z = ArrayDim - 1;
    row_y = ArrayDim - TargetDim;
    row_z = ArrayDim - 1;
}


    vector<bool> tempCol;
    vector<bool> compressedCol;

    int c = 0;
    for(int kevin=1; kevin<ArrayDim; kevin++){
        atoms = 0;
        for(j = row_y; j<=row_z; j++){
            atoms += RowTotals[j+1];
        }
        if(atoms>=TargetDim*TargetDim){
            break;
        }
        tempCol = ColumnAt(Array,kevin); // Extract desired column
        compressedCol = CompressRow(tempCol,col_y,col_z,RowTotals[kevin]);
        initCols.push_back(RearrangementMove());
        initCols[c].row = false;
        initCols[c].startingConfig = tempCol;
        initCols[c].endingConfig = compressedCol;
        initCols[c].dim = kevin;
        Array = assignCol(Array,compressedCol,kevin);
        c ++;
         // Push back column and compressed column
        RowTotals = RowSum(Array, ArrayDim); // Recalculate row totals
    }

    int s = 0;
    vector<int> Range1;
    vector<int> Range2;
    int balancedRows = 0;
    vector<vector<int>> RowRange = {{row_y,row_z}};

    while(balancedRows < row_z - row_y + 1){
        tie(Array,RowTotals,Range1,Range2,tempCols) = Balance(Array,RowRange[s],ArrayDim,TargetDim,RowTotals);
        cols.insert(cols.end(),tempCols.begin(),tempCols.end());
        if(Range1[1] == Range1[0]){
            balancedRows ++;
        }else{
            RowRange.push_back(Range1);
        }
        if(Range2[1] == Range2[0]){
            balancedRows ++;
        }else{
            RowRange.push_back(Range2);
        }
        s ++;
    }
    int counter = 0;
    i = RowRange[0][0];
    while(i <= RowRange[0][1]){
        rows.push_back(RearrangementMove());
        rows[counter].dim = i;
        rows[counter].row = true;
        rows[counter].startingConfig = Array[i];
        rows[counter].endingConfig = CompressRow(Array[i],col_y,col_z,RowTotals[i]);
        i++;
        counter++;
    }
    vector<RearrangementMove> r_moves = initCols;
    r_moves.insert(r_moves.end(),cols.begin(), cols.end());
    r_moves.insert(r_moves.end(),rows.begin(), rows.end());
return r_moves;
}


vector<RearrangementMove> rearrange(vector<vector<bool>> Array, enum rearrange_method method,int mode = 0)
{

    if(method == BALANCE_COMPRESS){
      return BalanceCompressAlg(Array,mode);
    }
    if(method == RECT_BALANCE_COMPRESS){
      return BalanceCompressAlg(Array,mode);
    }

    if(method == HUNGARIAN){
      return BalanceCompressAlg(Array,mode);
    }
}
