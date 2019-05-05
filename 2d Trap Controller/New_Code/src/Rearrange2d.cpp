#include "Rearrange2d_new.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <math.h>

using namespace std;

int i;
int j;
int k;
int COM[2];
int g_counter;
vector<RearrangementMove> moves;

void printArray();


//////////////////BALANCE_COMPRESS////////////////////////////


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

vector<int> RowSum(vector<vector<bool>> Array){
    vector<int> RowTotal(Array.size()+1);
    for(i=0; i < Array.size(); i++){
        for(j=0; j < Array[0].size(); j++){
            RowTotal[i+1] += int(Array[i][j]);
            RowTotal[0] += int(Array[i][j]);
        }
    }
    return RowTotal;
}

tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>> Balance(vector<vector<bool>> Array, vector<int> Range, int SufficientAtoms, vector<int> RowTotal){

    int dim = Array[0].size();
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
    moves[g_counter].dim = movefrom[1];
    moves[g_counter].row = false;
    moves[g_counter].startingConfig = ColumnAt(Array,movefrom[1]);
    Array[movefrom[0]][movefrom[1]] = false;
    Array[moveto[0]][moveto[1]] = true;
    moves[g_counter].endingConfig = ColumnAt(Array,movefrom[1]);
    g_counter ++;

    RowTotal[movefrom[0]+1] --;
    RowTotal[moveto[0]+1] ++;
    }
}
vector<int> Range1 = {center + 1,Range[1]};
vector<int> Range0 = {Range[0],center};
return make_tuple(Array,RowTotal,Range0,Range1);
}

vector<vector<bool>> assignCol(vector<vector<bool>> Array, vector<bool> col, int index)
{
    for(i = 0;i<Array.size();i++){
        Array[i][index] = col[i];
    }
    return Array;
}

int* CenterOfMass(vector<vector<bool>> Array){
    int dim1 = Array.size();
    int dim2 = Array[0].size();
    float RowWeight = 0;
    float ColWeight = 0;
    float TotalWeight = 0;
    for(i=0; i < dim1; i++){
        for(j=0; j < dim2; j++){
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

    int ArrayDim = Array.size();
    vector<int> RowTotals = RowSum(Array);
    int atoms = RowTotals[0];
    int TargetDim = sqrt(atoms);
    int col_y;
    int col_z;
    int row_y;
    int row_z;

//default mode set center of mass as center of array
if(mode == 0){
    int *COM = CenterOfMass(Array);

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
    int *COM = CenterOfMass(Array);
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

if(mode == 6 || mode == 7 || mode == 8){
    TargetDim = Array.size();
    row_y = 0;
    row_z = TargetDim - 1;
}

//mode 6 = rectangular, push left
if(mode == 6){
    col_y = 0;
    col_z = TargetDim - 1;
}

// mode 7 = rectangular, push right
if(mode == 7){
    col_y = Array[0].size() - TargetDim;
    col_z = Array[0].size() - 1;
}

//mode 8 = rectangular, center
if(mode == 8){
    int *COM = CenterOfMass(Array);
    col_y = COM[1] - TargetDim/2;
    col_z = col_y + TargetDim - 1;
}

    vector<bool> tempCol;
    vector<bool> compressedCol;
    bool check = false;
for(int kevin=1; kevin<ArrayDim && mode != (6||7||8); kevin++){
        atoms = 0;
        for(j = row_y; j<=row_z; j++){
            atoms += RowTotals[j+1];
        }
        if(atoms>=TargetDim*TargetDim){
            check = true;
            break;
        }
        tempCol = ColumnAt(Array,kevin); // Extract desired column
        compressedCol = CompressRow(tempCol,col_y,col_z,RowTotals[kevin]);
        moves.push_back(RearrangementMove());
        moves[g_counter].row = false;
        moves[g_counter].startingConfig = tempCol;
        moves[g_counter].endingConfig = compressedCol;
        moves[g_counter].dim = kevin;
        Array = assignCol(Array,compressedCol,kevin);
        g_counter ++;
         // Push back column and compressed column
        RowTotals = RowSum(Array); // Recalculate row totals
    }

if(check == false && mode != (6||7||8)){
    col_z --;
    row_z --;
    TargetDim --;
}

    int s = 0;
    vector<int> Range1;
    vector<int> Range2;
    int balancedRows = 0;
    vector<vector<int>> RowRange = {{row_y,row_z}};

    //balance rows until #of balanced rows = target dim
    while(balancedRows < TargetDim){
        tie(Array,RowTotals,Range1,Range2) = Balance(Array,RowRange[s],TargetDim,RowTotals);
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
    i = RowRange[0][0];
    while(i <= RowRange[0][1]){
        moves.push_back(RearrangementMove());
        moves[g_counter].dim = i;
        moves[g_counter].row = true;
        moves[g_counter].startingConfig = Array[i];
        moves[g_counter].endingConfig = CompressRow(Array[i],col_y,col_z,RowTotals[i]);
        i++;
        g_counter++;
    }

return moves;
}

///////////////////////////HUNGARIAN///////////////////////////////////////

































///////////////////////////////////END////////////////////////////////

vector<RearrangementMove> rearrange(vector<vector<bool>> Array, enum rearrange_method method,int mode)
{

    if(method == BALANCE_COMPRESS){
      return BalanceCompressAlg(Array,mode);
    }

    if(method == HUNGARIAN){
      return BalanceCompressAlg(Array,mode);
    }
}
