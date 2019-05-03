#include "Rearrange2d.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <math.h>
#include "rearrangementmove.h"


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
  for(k=0;i<Array.size();k++)
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
    for(i=1; i < dimension; i++){
        for(j=0; j < dimension; j++){
            RowTotal[i] += int(Array[i][j]);
            RowTotal[0] += RowTotal[i];
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
    int RT = int(RowWeight /= TotalWeight);
    int CT = int(ColWeight /= TotalWeight);
    return {RT,CT};
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

tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<vector<vector<int>>>,vector<int>,vector<vector<bool>>,vector<vector<bool>>> Balance(vector<vector<bool>> Array, vector<int> Range, vector<int> COM, int dim, int SufficientAtoms, vector<int> RowTotal){
    vector<vector<vector<int>>> moves;

    vector<vector<bool>> oldCols;
    vector<vector<bool>> newCols;
    vector<int> cols;


    int center;
    if((Range[1] - Range[0])%2 == 0){
        center  = (Range[1] - Range[0])/2 + Range[0];
    }else{
        center  = (Range[1] - Range[0] - 1)/2 + Range[0];
    }
    int SuffLower = (center - Range[0] + 1)*SufficientAtoms;
    int SuffUpper = (Range[1] - center)*SufficientAtoms;

    int k;
    int i = Range[0];
    int Lower = 0;
    int Upper = 0;
    while(i <= center){
        Lower += RowTotal[i];
        i ++;
    }
    while(i <= Range[1]){
        Upper += RowTotal[i];
        i ++;
    }

    while(SuffUpper > Upper || SuffLower > Lower){
        i = center + 1;
        vector<int> moveto ;
        vector<int> movefrom ;
        int j = 0;
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
                        moveto ;
                        movefrom ;
                    }else{
                        Lower ++;
                        break;
                }
        }
    }
    if(moveto.size() != 0 || movefrom.size() != 0){
    vector<vector<bool>> New = Move(Array,movefrom,moveto);

    oldCols.push_back(Transpose(Array)[movefrom[1]]);
    newCols.push_back(Transpose(New)[movefrom[1]]);
    cols.push_back(movefrom[1]);

    Array = New;

    vector<vector<vector<int>>> new_moves = Interpolate(movefrom, moveto,center);
    for(int i = 0;i<new_moves.size();i++){
        moves.push_back(new_moves[i]);
    }

    RowTotal[movefrom[0]] --;
    RowTotal[moveto[0]] ++;
    }
}
vector<int> Range1 = {center + 1,Range[1]};
vector<int> Range0 = {Range[0],center};
return make_tuple(Array,RowTotal,Range0,Range1,moves,cols,oldCols,newCols);
}

vector<RearrangementMove> BalanceCompressAlg(vector<vector<bool>> Array){

  vector<RearrangementMove> init_rows;
  vector<RearrangementMove> rows;
  vector<RearrangementMove> cols;


    vector<vector<vector<int>>> moves2;

    vector<vector<bool>> oldRows ;
    vector<vector<bool>> newRows ;
    vector<vector<bool>> oldCols ;
    vector<vector<bool>> newCols ;
    vector<int> cols;

    vector<vector<bool>> initOldCols ;
    vector<vector<bool>> initNewCols ;
    vector<int> initCols;

    vector<vector<bool>> oldCols1 ;
    vector<vector<bool>> newCols1 ;
    vector<int> cols1;

    int ArrayDim = Array.size();

    int* COM = CenterOfMass(Array, ArrayDim);
    vector<int> RowTotals = RowSum(Array, ArrayDim);

    int atoms = RowTotals[0];
    vector<vector<int>> RowRange ;


    int TargetDim = sqrt(atoms)
    float x = TargetDim/2;


        int col_y = COM[1] - x;
        int col_z = COM[1] + x - 1;

        if(col_y < 0){
            col_y = 0;
            col_z = TargetDim - 1;
        }
        if(col_z >= ArrayDim){
            col_z = ArrayDim - 1;
            col_y = ArrayDim - TargetDim;
        }

        row_y = COM[0] - x;
        row_z = COM[0] + x - 1;

        if(row_y < 0){
            row_y = 0;
            row_z = TargetDim - 1;
        }
        if(row_z >= ArrayDim){
            row_z = ArrayDim - 1;
            row_y = ArrayDim - TargetDim;
        }

    vector<bool> tempCol;

    for(i=1; i<ArrayDim; i++){
        tempCol = ColumnAt(Array,i); // Extract desired column
        init_cols.push_back(tempCol,CompressRow(tempCol,COM[1],TargetDim),0,i); \\Push back column and compressed column
        RowTotals = RowSum(Array, ArrayDim); // Recalculate row totals
        atoms = 0;
        for(j = row_y; j<row_z; j++){
            atoms += RowTotals[j];
        }
        if(atoms>=TargetDim*TargetDim){
            break;
        }
    }

    i = 0;
    vector<int> Range1;
    vector<int> Range2;
    vector<int> BalancedRows ;
    vector<vector<vector<int>>> newmoves;
    int k = 0;

    while(BalancedRows.size() < row_z - row_y + 1){
    tie(Array,RowTotals,Range1,Range2,newmoves,cols1,oldCols1,newCols1) = Balance(Array,[row_y,row_z],COM,ArrayDim,TargetDim,RowTotals);

    for(int l = 0;l<cols1.size();l++){
        cols.push_back(cols1[l]);
        oldCols.push_back(oldCols1[l]);
        newCols.push_back(newCols1[l]);
    }

    while(k<newmoves.size()){
        moves2.push_back(newmoves[k]);
        k ++;
    }
    newmoves ;
    k = 0;
    vector<vector<int>> RowRange{{row_y,row_z}};
    if(Range1[1] == Range1[0]){
        BalancedRows.push_back(Range1[1]);
    }else{
        RowRange.push_back(Range1);
    }
    if(Range2[1] == Range2[0]){
        BalancedRows.push_back(Range2[1]);
    }else{
        RowRange.push_back(Range2);
    }
    i ++;
    }

    i = RowRange[0][0];
    vector<int> rows;

    while(i <= RowRange[0][1]){
        vector<bool> NewRow = CompressRow(Array[i],COM[1],TargetDim);
        oldRows.push_back(Array[i]);
        newRows.push_back(NewRow);
        rows.push_back(i);
        Array[i] = NewRow;
        i++;
    }

return make_tuple(initCols,initOldCols,initNewCols,rows,oldRows,newRows,cols,oldCols,newCols);
}

enum rearrange_method {BALANCE_COMPRESS,RECT_BALANCE_COMPRESS, SNAKE, HUNGARIAN};

vector<RearrangementMove> rearrange(vector<vector<bool>> Array, enum rearrange_method method)
{

    if(method == BALANCE_COMPRESS){
      return BalanceCompressAlg(Array);
    }
    if(method == RECT_BALANCE_COMPRESS){
      return BalanceCompressAlg(Array);
    }
    if(method == SNAKE){
      return BalanceCompressAlg(Array);
    }
    if(method == HUNGARIAN){
      return BalanceCompressAlg(Array);
    }

// tie(initCols,initOldCols,initNewCols,rows,oldRows,newRows,cols,oldCols,newCols) = BalanceCompressAlg(Array);


    // vector<RearrangementMove> moves;
    // for(int i = 0;i<initCols.size();i++){
    //     moves.push_back(RearrangementMove());
    //     moves[i].startingConfig = initOldCols[i];
    //     moves[i].endingConfig = initNewCols[i];
    //     moves[i].row = false; // 1 if row, 0 if col
    //     moves[i].dim = initCols[i]; // the index of the desired move
    // }
    // for(int i =initCols.size();i<(cols.size() + initCols.size());i++){
    //     moves.push_back(RearrangementMove());
    //     moves[i].startingConfig = oldCols[i- initCols.size()];
    //     moves[i].endingConfig = newCols[i- initCols.size()];
    //     moves[i].row = false; // 1 if row, 0 if col
    //     moves[i].dim = cols[i- initCols.size()]; // the index of the desired move
    // }
    // for(int i = cols.size() + initCols.size();i<(rows.size() + cols.size()+initCols.size());i++){
    //     moves.push_back(RearrangementMove());
    //     moves[i].startingConfig = oldRows[i - cols.size() - initCols.size()];
    //     moves[i].endingConfig = newRows[i - cols.size()- initCols.size()];
    //     moves[i].row = true; // 1 if row, 0 if col
    //     moves[i].dim = rows[i - cols.size()- initCols.size()]; // the index of the desired move
    // }


}
