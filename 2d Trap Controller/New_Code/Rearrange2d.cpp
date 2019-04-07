#include "Rearrange2d.h"
#include "BalanceCompress.h"

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

vector<vector<bool>> MakeBoolArray(int Dim, float Prob){
    int i = 0;
    int j = 0;
    vector<vector<bool>> matrix;
    vector<bool> row;
    while (i < Dim){
        while(j < Dim){
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            if (r <= Prob){
                row.push_back(true);
            }else{
                row.push_back(false);
            }
            j++;
        }
        i++;
        j = 0;
        matrix.push_back(row);
        row = {};

    }
    return matrix;
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

tuple<vector<bool>,vector<vector<int>>> CompressRow(vector<bool> row, int center, int suff){
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
    vector<int> pos = {};
    for(int i = 0;i<dim;i++){
        if( row[i] == true){
            atoms ++;
            pos.push_back(i);
        }
    }
    int diff = atoms - suff;
    if(diff<0){
        cout << "error! insufficient atoms in row" << endl;
    }
    vector<vector<int>> moves = {};
    int j = 0;
    int k = 0;
    vector<bool> newRow = {};
    int extras = diff;

    while(j<diff and j<r1){
        newRow.push_back(true);
        moves.push_back({pos[k],j});
        j ++;
        k ++;
        extras--;
    }
    while(j<r1){
        newRow.push_back(false);
        j++;
    }
    while(j<=r2){
        newRow.push_back(true);
        moves.push_back({pos[k],j});
        k++;
        j++;
    }
    while(j<dim-extras){
        newRow.push_back(false);
        j++;
    }
    while(j<dim){
        newRow.push_back(true);
        moves.push_back({pos[k],j});
        k++;
        j++;
    }
    return make_tuple(newRow,moves);
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

vector<vector<bool>> Transpose(vector<vector<bool>> Array, int dim){
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
        row = {};
        j = 0;
        i++;
    }
    return NewArray;
}

tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<vector<vector<int>>>> Balance(vector<vector<bool>> Array, vector<int> Range, vector<int> COM, int dim, int SufficientAtoms, vector<int> RowTotal){
    vector<vector<vector<int>>> moves;
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
        vector<int> moveto = {};
        vector<int> movefrom = {};
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
    Array = Move(Array,movefrom,moveto);

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
return make_tuple(Array,RowTotal,Range0,Range1,moves);
}

tuple<vector<vector<vector<int>>>,vector<int>, vector<vector<vector<int>>>> BalanceCompressAlg(vector<vector<bool>> Array, int TargetDim){
    vector<vector<vector<int>>> moves2;

    int ArrayDim = Array.size();

    vector<int> COM = CenterOfMass(Array, ArrayDim);
    vector<int> RowTotals = RowSum(Array, ArrayDim);
    int i = 0;
    int atoms = 0;
    vector<vector<int>> RowRange = {};
    float x = TargetDim;
    x/=2;
    if(TargetDim == 0){
        while(i < ArrayDim){
            atoms += RowTotals[i];
            i ++;
        }
        TargetDim = atoms/ArrayDim;
        RowRange.push_back({0,ArrayDim - 1});
    }else{
        int r1 = COM[0] - x;
        int r2 = COM[0] + x - 1;
        if(r1 < 0){
            r1 = 0;
            r2 = TargetDim - 1;
        }
        if(r2 >= ArrayDim){
            r2 = ArrayDim - 1;
            r1 = ArrayDim - TargetDim;
        }
        RowRange.push_back({r1,r2});
    }

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

    i = 0;
    vector<int> Range1;
    vector<int> Range2;
    vector<int> BalancedRows = {};
    vector<vector<vector<int>>> newmoves;
    int k = 0;

    while(BalancedRows.size() < abs(RowRange[0][1] - RowRange[0][0] + 1)){
    tie(Array,RowTotals,Range1,Range2,newmoves) = Balance(Array,RowRange[i],COM,ArrayDim,TargetDim,RowTotals);
    while(k<newmoves.size()){
        moves2.push_back(newmoves[k]);
        k ++;
    }
    newmoves = {};
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
    vector<int> rows = {};
    vector<vector<vector<int>>> rowmoves = {};
    for(int l = 0;l<ArrayDim;l++){
        rowmoves.push_back({{0,0}});
    }

    while(i <= RowRange[0][1]){
        tie(Array[i],rowmoves[i]) = CompressRow(Array[i],COM[1],TargetDim);
        rows.push_back(i);
        i++;
    }

return make_tuple(moves2,rows, rowmoves);
}
