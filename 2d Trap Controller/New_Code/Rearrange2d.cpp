#include "Rearrange2d.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <math.h>
#include "rearrangementmove.h"


using namespace std;

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

vector<int> CenterOfMass(vector<vector<bool>> Array, int Dim){
    int i = 0;
    int j = 0;
    float RowWeight = 0;
    float ColWeight = 0;
    float TotalWeight = 0;
    while(i < Dim){
        while(j < Dim){
            if (Array[i][j] == true){
                RowWeight += i;
                ColWeight += j;
                TotalWeight ++;
            }
            j += 1;
        }
        i += 1;
        j = 0;
    }
    RowWeight /= TotalWeight;
    ColWeight /= TotalWeight;
    vector<int> COM = {RowWeight,ColWeight};
    return COM;
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

vector<int> RowSum(vector<vector<bool>> Array, int dim){
    vector<int> RowTotal;
    int dummy = 0;
    int i = 0;
    int j = 0;
    while(i < dim){
        while(j < dim){
            if(Array[i][j] == true){
                dummy ++;
            }
            j ++;
        }
        RowTotal.push_back(dummy);
        dummy = 0;
        j = 0;
        i ++;
    }
    return RowTotal;
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

tuple<vector<int>,vector<vector<bool>>, vector<vector<bool>>,vector<int>,vector<vector<bool>>, vector<vector<bool>>,vector<int>,vector<vector<bool>>, vector<vector<bool>>> BalanceCompressAlg(vector<vector<bool>> Array){
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
    int TargetDim;

    vector<int> COM = CenterOfMass(Array, ArrayDim);
    vector<int> RowTotals = RowSum(Array, ArrayDim);

    int i = 0;
    int atoms = 0;
    vector<vector<int>> RowRange ;


        while(i < ArrayDim){
            atoms += RowTotals[i];
            i ++;
        }

        TargetDim = sqrt(atoms);


    float x = TargetDim;
    x/=2;


        int y = COM[1] - x;
        int z = COM[1] + x - 1;
        vector<int> ColRange = {y,z};

        if(ColRange[0] < 0){
            ColRange[0] = 0;
            ColRange[1] = TargetDim - 1;
        }
        if(ColRange[1] >= ArrayDim){
            ColRange[1] = ArrayDim - 1;
            ColRange[0] = ArrayDim - TargetDim;
        }

        y = COM[0] - x;
        z = COM[0] + x - 1;
        RowRange = {{y,z}};

        if(RowRange[0][0] < 0){
            RowRange[0][0] = 0;
            RowRange[0][1] = TargetDim - 1;
        }
        if(RowRange[0][1] >= ArrayDim){
            RowRange[0][1] = ArrayDim - 1;
            RowRange[0][0] = ArrayDim - TargetDim;
        }

    bool done = false;
    i = 1;
    while(!done){
        Array = Transpose(Array);
        vector<bool> tempCol = CompressRow(Array[i],COM[1],TargetDim);
        initCols.push_back(i);
        initOldCols.push_back(Array[i]);
        initNewCols.push_back(tempCol);
        Array[i] = tempCol;
        Array = Transpose(Array);
        RowTotals = RowSum(Array, ArrayDim);
        atoms = 0;
        for(int k = RowRange[0][0];k<RowRange[0][1];k++){
            atoms += RowTotals[k];
        }
        if(atoms>=TargetDim*TargetDim){
            done = true;
        }
        i ++;
        if(i==ArrayDim){
            break;
        }
    }

    i = 0;
    vector<int> Range1;
    vector<int> Range2;
    vector<int> BalancedRows ;
    vector<vector<vector<int>>> newmoves;
    int k = 0;

    while(BalancedRows.size() < RowRange[0][1] - RowRange[0][0] + 1){
    tie(Array,RowTotals,Range1,Range2,newmoves,cols1,oldCols1,newCols1) = Balance(Array,RowRange[i],COM,ArrayDim,TargetDim,RowTotals);

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
    vector<vector<bool>> oldRows;
    vector<vector<bool>> newRows;
    vector<vector<bool>> oldCols;
    vector<vector<bool>> newCols;
    vector<vector<bool>> initNewCols;
    vector<vector<bool>> initOldCols;
    vector<int> initCols;
    vector<int> cols;
    vector<int> rows;

    if(method == BALANCE_COMPRESS){
    tie(initCols,initOldCols,initNewCols,rows,oldRows,newRows,cols,oldCols,newCols) = BalanceCompressAlg(Array);
    }
    if(method == RECT_BALANCE_COMPRESS){

    }
    if(method == SNAKE){

    }
    if(method == HUNGARIAN){

    }

    vector<RearrangementMove> moves;
    for(int i = 0;i<initCols.size();i++){
        moves.push_back(RearrangementMove());
        moves[i].startingConfig = initOldCols[i];
        moves[i].endingConfig = initNewCols[i];
        moves[i].row = false; // 1 if row, 0 if col
        moves[i].dim = initCols[i]; // the index of the desired move
    }
    for(int i =initCols.size();i<(cols.size() + initCols.size());i++){
        moves.push_back(RearrangementMove());
        moves[i].startingConfig = oldCols[i- initCols.size()];
        moves[i].endingConfig = newCols[i- initCols.size()];
        moves[i].row = false; // 1 if row, 0 if col
        moves[i].dim = cols[i- initCols.size()]; // the index of the desired move
    }
    for(int i = cols.size() + initCols.size();i<(rows.size() + cols.size()+initCols.size());i++){
        moves.push_back(RearrangementMove());
        moves[i].startingConfig = oldRows[i - cols.size() - initCols.size()];
        moves[i].endingConfig = newRows[i - cols.size()- initCols.size()];
        moves[i].row = true; // 1 if row, 0 if col
        moves[i].dim = rows[i - cols.size()- initCols.size()]; // the index of the desired move
    }

    return moves;
}
