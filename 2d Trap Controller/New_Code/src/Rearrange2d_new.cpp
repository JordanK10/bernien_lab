#include "C:\Users\abpoc\Desktop\bernien_lab\2d Trap Controller\New_Code\include\Rearrange2d.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <math.h>



using namespace std;

int i;
int j;
int k;

vector<vector<vector<int>>> Interpolate(vector<int> movefrom, vector<int> moveto, int center){
    vector<vector<vector<int>>> moves;
    if(moveto[0] > movefrom[0]){
        for(int i = 0;i<(moveto[0] - center - 1);i++){
            moves.push_back({{moveto[0] -i - 1,moveto[1]},{moveto[0] - i,moveto[1]}});
        }
        moves.push_back({movefrom,{center + 1,moveto[1]}});
    }else{
        for(int i = 0;i<(center - moveto[0]);i++){
            moves.push_back({{moveto[0]+i+1,moveto[1]},{moveto[0]+i,moveto[1]}});
        }
        moves.push_back({movefrom,{center,moveto[1]}});
    }
    return moves;
}

vector<bool> ColumnAt(vector<vector<bool>> Array, int dim){
  vector<bool> new_col(Array.size());
  for(k=0;k<Array.size();k++)
    new_col[k]=Array[k][dim];
  return new_col;
}

vector<bool> CompressRow(vector<bool> row, int center, int suff){
    int dim = row.size();
    float x = suff;
    x/=2;
    int r1 = round(center - x);
    int r2 = r1 + suff - 1;

    if(r1<0){
        r1 = 0;
        r2 = suff - 1;
    }
    if(r2>=dim){
        r1 = dim - suff;
        r2 = dim -1;
    }

    int atoms = 0;
    for(int i = 0;i<dim;i++){
        if( row[i] == true){
            atoms ++;
        }
    }
    int diff = atoms - suff;

    vector<vector<int>> moves;
    int j = 0;
    vector<bool> newRow ;
    int extras = diff;
    while(j<diff && j<r1){
        newRow.push_back(true);
        j ++;
        extras--;

    }
    while(j<r1){
        newRow.push_back(false);
        j++;
    }
    while(j<=r2){
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

vector<vector<bool>> Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2){
    vector<vector<bool>> NewArray = Array;
    NewArray[pos1[0]][pos1[1]] = false;
    NewArray[pos2[0]][pos2[1]] = true;
    return NewArray;
}

vector<int> RowSum(vector<vector<bool>> Array, int dimension){
    vector<int> RowTotal(dimension+1);
    int dummy = 0;
    for(i=0; i < dimension; i++){
        for(j=0; j < dimension; j++){
            RowTotal[i+1] += int(Array[i][j]);
            RowTotal[0] += int(Array[i][j]);
        }
    }
    return RowTotal;
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
    int COM[] = {int(RowWeight /= TotalWeight),int(ColWeight /= TotalWeight)};
    return COM;
}

vector<vector<bool>> Transpose(vector<vector<bool>> Array){
    int dim = Array.size();
    int i = 0;
    int j = 0;
    vector<vector<bool>> NewArray;
    vector<bool> row;
    while(i < dim){
        while(j < dim){
            row.push_back(Array[j][i]);
            j ++;
        }
        NewArray.push_back(row);
        row ;
        j = 0;
        i++;
    }
    return NewArray;
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
    cout << Range[0];
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
                        moveto ;
                        movefrom ;
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
    Array = Move(Array,movefrom,moveto);
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

void printArray(vector<vector<bool>> Array){
    for(int i = 0;i<Array.size();i++){
        for(int j = 0;j<Array[0].size();j++){
            cout << Array[i][j] << " ";
        }
        cout << endl;
    }
cout << endl;
}
vector<RearrangementMove> BalanceCompressAlg(vector<vector<bool>> Array){

  vector<RearrangementMove> initCols;
  vector<RearrangementMove> rows;
  vector<RearrangementMove> cols;
  vector<RearrangementMove> tempCols;


    int ArrayDim = Array.size();
/*
    int* COM;
    COM = CenterOfMass(Array, ArrayDim);
    cout << "nx: "<< *(COM) << endl;
    cout << "y: " << *(COM + 1) << endl;

*/
    vector<int> RowTotals = RowSum(Array, ArrayDim);

    int atoms = RowTotals[0];

    int COM[] = {5,5};

    int TargetDim = sqrt(atoms);
    cout << atoms;
    cout << "Tdim: " << TargetDim << endl;
    float x = TargetDim/2;


        int col_y = COM[1] - x;
        int col_z = TargetDim + col_y - 1;

        if(col_y < 0){
            col_y = 0;
            col_z = TargetDim - 1;
        }
        if(col_z >= ArrayDim){
            col_z = ArrayDim - 1;
            col_y = ArrayDim - TargetDim;
        }

        int row_y = COM[0] - x;
        int row_z = TargetDim + row_y - 1;

        if(row_y < 0){
            row_y = 0;
            row_z = TargetDim - 1;
        }
        if(row_z >= ArrayDim){
            row_z = ArrayDim - 1;
            row_y = ArrayDim - TargetDim;
        }

    vector<bool> tempCol;
    vector<bool> compressedCol;

    cout << row_y << "," << row_z << endl;
    cout << "ArrayDim" << ArrayDim << endl;
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
        compressedCol = CompressRow(tempCol,COM[0],TargetDim);
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
    int k = 0;
    vector<vector<int>> RowRange = {{row_y,row_z}};


    while(balancedRows < row_z - row_y + 1){
        cout << "Range: " << RowRange[s][0] << " " << RowRange[s][1] << endl;

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
        rows[counter].endingConfig = CompressRow(Array[i],COM[1],TargetDim);
        i++;
        counter++;
    }
    vector<RearrangementMove> r_moves = initCols;
    r_moves.insert(r_moves.end(),cols.begin(), cols.end());
    r_moves.insert(r_moves.end(),rows.begin(), rows.end());
return r_moves;
}


vector<RearrangementMove> rearrange(vector<vector<bool>> Array, enum rearrange_method method)
{

    if(method == BALANCE_COMPRESS){
      return BalanceCompressAlg(Array);
    }
    if(method == RECT_BALANCE_COMPRESS){
      return BalanceCompressAlg(Array);
    }

    if(method == HUNGARIAN){
      return BalanceCompressAlg(Array);
    }
}
