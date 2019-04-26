#include "Rearrange2d.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <ctime>
#include <math.h>
#include "rearrangementmove.h"


using namespace std;

///////////////////////Balance Compress///////////////////////
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
    int r1 = (center - x);
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
    int height = Array.size();
    int width = Array[0].size();
    int i = 0;
    int j = 0;
    vector<vector<bool>> NewArray;
    vector<bool> row;
    while(i < width){
        while(j < height){
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



///////////////////////Rect Balance Compress///////////////////////
vector<int> RCenterOfMass(vector<vector<bool>> Array, int Height, int Width){
    int i = 0;
    int j = 0;
    float RowWeight = 0;
    float ColWeight = 0;
    float TotalWeight = 0;
    while(i < Height){
        while(j < Width){
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

bool Toggle(bool a){
    bool b;
    if(a == false){
        b = true;
    }else{
        b = false;
    }
    return b;
}

vector<vector<vector<int>>> Bank(vector<vector<bool>> Array, int Range){
    vector<vector<vector<int>>> moves = {};
    int Height = Array.size();
    vector<vector<int>> pos = {};

    for(int j = 0; j < Range;j++){
        for(int i = 0; i < Height; i++){
            if(Array[i][j] == true){
                pos.push_back({i,j});
            }
        }
    }

    vector<vector<int>> path = {};
    bool invert = true;
    int i = 0;
    int j = 0;

    while(i<Range){
        while(j<Height && j>-1){
            path.push_back({j,i});
            if(invert == true){
                j ++;
            }else{
                j --;
            }
        }
        i ++;
        if(invert == true){
            j --;
        }else{
            j ++;
        }
        invert = Toggle(invert);
    }
    i = 0;
    j = 0;
    int n = pos.size();
    while(i < n){
        if(Array[path[j][0]][path[j][1]] == true){
            moves.push_back({path[j],path[i]});
            i ++;
        }
        j ++;
    }
    return moves;
}


vector<int> RRowSum(vector<vector<bool>> Array, int H, int W){
    vector<int> RowTotal;
    int dummy = 0;
    int i = 0;
    int j = 0;
    while(i < H){
        while(j < W){
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

tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<vector<vector<int>>>,vector<int>, vector<vector<bool>>, vector<vector<bool>>> RBalance(vector<vector<bool>> Array, vector<int> Range, vector<int> COM, int Height,int Width, vector<int> RowTotal){
    vector<vector<vector<int>>> moves;

    vector<int> cols;
    vector<vector<bool>> oldCols;
    vector<vector<bool>> newCols;

    int SufficientAtoms = Height;
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
            while(j < Width){
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
                        moveto = {};
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
            while(j < Width){
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
                        moveto = {};
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

tuple<vector<int>,vector<vector<bool>>,vector<vector<bool>>,vector<int>,vector<vector<bool>>,vector<vector<bool>>> RBalanceCompress(vector<vector<bool>> Array){
    vector<vector<vector<int>>> moves2;

    vector<vector<bool>> oldRows ;
    vector<vector<bool>> newRows ;
    vector<vector<bool>> oldCols ;
    vector<vector<bool>> newCols ;
    vector<int> cols;
    vector<int> rows;

    vector<int> cols1;
    vector<vector<bool>> oldCols1;
    vector<vector<bool>> newCols1;


    int Height = Array.size();
    int Width = Array[0].size();

    vector<int> COM = RCenterOfMass(Array, Height, Width);
    vector<int> RowTotals = RRowSum(Array, Height, Width);
    int atoms  = 0;
    bool check1 = true;
    for(int y = 0;y<Height;y++){
        if(RowTotals[y]<Height){
            check1 = false;
        }
        atoms += RowTotals[y];
    }

    vector<vector<int>> RowRange = {{0,Height - 1}};
    float x = Height;
    x -= 1;
    x/=2;
    int y = COM[1] - x;
    int z = y + Height - 1;
    vector<int> ColRange = {y,z};

        if(ColRange[0] < 0){
            ColRange[0] = 0;
            ColRange[1] = Height - 1;
        }
        if(ColRange[1] >= Width){
            ColRange[1] = Width - 1;
            ColRange[0] = Width - Height;
        }

    int i = 0;
    vector<int> Range1;
    vector<int> Range2;
    vector<int> BalancedRows = {};
    vector<vector<vector<int>>> newmoves;

    while(BalancedRows.size() < Height){
        tie(Array,RowTotals,Range1,Range2,newmoves,cols1,oldCols1,newCols1) = RBalance(Array,RowRange[i],COM,Height,Width,RowTotals);

        for(int l = 0;l<cols1.size();l++){
            cols.push_back(cols1[l]);
            oldCols.push_back(oldCols1[l]);
            newCols.push_back(newCols1[l]);
        }
        for(int k = 0; k<newmoves.size(); k++){
            moves2.push_back(newmoves[k]);
        }

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

    while(i <= RowRange[0][1]){
        vector<bool> NewRow = CompressRow(Array[i],COM[1],Height);
        oldRows.push_back(Array[i]);
        newRows.push_back(NewRow);
        rows.push_back(i);
        Array[i] = NewRow;
        i++;
    }

return make_tuple(rows,oldRows,newRows,cols,oldCols,newCols);
}

////////////////////////HUNGARIAN/////////////////////////


tuple<vector<bool>,vector<bool>> clear_covers(int n){
    vector<bool> a = {};
    vector <bool> b = {};
    for(int i = 0; i < n; i ++){
        a.push_back(false);
        b.push_back(false);
    }
    return make_tuple(a,b);
}



bool Check(vector<vector<bool>> Array, vector<int> RowRange, vector<int> ColRange){
    int i = RowRange[0];
    int j = ColRange[0];
    bool check = true;
    while(i<RowRange[1]){
        while(j<ColRange[1]){
            if(Array[i][j] != true){
                check = false;
            }
            j ++;
        }
        j = ColRange[0];
        i ++;
    }
    return check;
}



tuple<int,int> find_zero(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered){
            signed int row = -1;
            signed int col = -1;
            int i = 0;
            bool done = false;
            int j;

            while(done == false){
                j = 0;
                while(1){
                    if(CostMatrix[i][j] == 0 && row_covered[i] == false && col_covered[j] == false){
                        row = i;
                        col = j;
                        done = true;
                    }
                    j += 1;
                    if(j>=n){
                        break;
                    }
                }
                i += 1;
                if(i>= n){
                    done = true;
                }
            }
            return make_tuple(row, col);
        }

int find_star_row(vector<vector<int>> marked, int row, int n){
        signed int col = -1;
        for(int j = 0; j<n;j++){
            if(marked[row][j] == 1){
                col = j;
                break;
            }
        }
        return col;
}

int find_star_col(vector<vector<int>> marked, int col, int n){
        signed int row = -1;
        for(int j = 0; j<n;j++){
            if(marked[j][col] == 1){
                row = j;
                break;
            }
        }
        return row;
}

int find_prime_row(int n, int row, vector<vector<int>> marked){
        signed int col = -1;
        for(int j = 0;j<n;j++){
            if(marked[row][j] == 2){
                col = j;
                break;
            }
        }
        return col;
}

vector<vector<int>> erase_primes(int n, vector<vector<int>> marked){
        for(int i = 0; i<n; i ++){
            for(int j = 0; j<n;j++){
                if(marked[i][j] == 2){
                    marked[i][j] = 0;
                }
            }
        }
    return marked;
}                int atoms = 0;

vector<vector<int>> convert_path(int counter, vector<vector<int>> path, int n, vector<vector<int>> marked){
    for(int i = 0; i<counter + 1; i++){
        if(marked[path[i][0]][path[i][1]] == 1){
            marked[path[i][0]][path[i][1]] = 0;
        }else{
            marked[path[i][0]][path[i][1]] = 1;
        }
    }
    return marked;
}

int find_smallest(vector<vector<int>> CostMatrix, int n, vector<bool> row_covered, vector<bool> col_covered){
        int minval = n*n;
        for(int i = 0; i<n; i++){
            for(int j = 0; j<n; j++){
                if(row_covered[i] == false && col_covered[j] == false){
                    if(minval > CostMatrix[i][j]){
                        minval = CostMatrix[i][j];
                    }
                }
            }
        }
        return minval;
}
int metric(vector<int> pos1, vector<int> pos2,int dim){
    signed int x = pos1[0] - pos2[0];
    signed int y = pos1[1] - pos2[1];
    int distance = 0;
    if(x<0){
        x = -x;
    }
    if(y<0){
        y = -y;
    }
    if(x!=0 && y!=0){
        distance += 2*dim*dim;
    }
    distance += x*x + y*y;
    return distance;
}

tuple<vector<vector<int>>,vector<vector<int>>, int> ToCostMatrix(vector<vector<bool>> InitArray, vector<vector<int>> TargetArray, int Dim1){
    vector<vector<int>> InitPositions = {};
    int NumberOfPositions = TargetArray.size();
    int i = 0;
    int j = 0;
    int atoms = 0;
    while(i<Dim1){
        while(j<Dim1){
            if(InitArray[i][j] == true){
                InitPositions.push_back({i,j});
                atoms += 1;
            }
            j += 1;
        }
        i += 1;
        j = 0;
    }
    i = 0;
    j = 0;
    vector<int> row = {};
    vector<vector<int>> CostMatrix = {};
    int difference = atoms - NumberOfPositions;
    if(difference>=0){
        while(i<NumberOfPositions){
            while(j<atoms){
                int dist = metric(InitPositions[j],TargetArray[i],Dim1);
                row.push_back(dist);
                j += 1;
            }
            CostMatrix.push_back(row);
            j = 0;
            i += 1;
            row = {};
        }
        i = 0;
        row = {};
        j = 0;
        while(j<atoms){
            row.push_back(0);
            j += 1;
        }
        while(i < difference){
            CostMatrix.push_back(row);
            i += 1;
        }
    }
    return make_tuple(CostMatrix, InitPositions, difference);
}

tuple<vector<vector<int>>, vector<int>, vector<int>> ToTargetArray(int dim, vector<float> COM){
    float x = dim;
    x -= 1;
    x /= 2;
    vector<int> RowRange = {COM[0] - x, COM[0] + x};
    int r0 = RowRange[0];
    int r1 = RowRange[1];
    if(RowRange[0] - r0 > 0.5){
        r0 += 1;
    }
    if(RowRange[1] - r1 > 0.5){
        r1 += 1;
    }
    RowRange = {r0,r1};

    vector<int> ColRange = {COM[1] - x, COM[1] + x};
    int c0 = ColRange[0];
    int c1 = ColRange[1];
    if(ColRange[0] - r0 > 0.5){
        c0 += 1;
    }
    if(ColRange[1] - r1 > 0.5){
        c1 += 1;
    }
    ColRange = {c0,c1};

    vector<vector<int>> TargetArray = {};
    int i = RowRange[0];
    int j = ColRange[0];
    while(i <= RowRange[1]){
        while(j <= ColRange[1]){
            TargetArray.push_back({i,j});
            j+= 1;
        }
        i += 1;
        j = ColRange[0];
    }
    return make_tuple(TargetArray,RowRange,ColRange);
}

vector<float> HCenterOfMass(vector<vector<bool>> Array, int dim){
        int i = 0;
        int j = 0;
        float RowWeight = 0.0;
        float ColWeight = 0.0;
        float TotalWeight = 0.0;
        while(i < dim){
            while(j<dim){
                if(Array[i][j] == true){
                    RowWeight += i;
                    ColWeight += j;
                    TotalWeight += 1;
                }
                j += 1;
            }
            j = 0;
            i ++;
        }
        RowWeight /= TotalWeight;
        ColWeight /= TotalWeight;
        return {RowWeight,ColWeight};
}

vector<vector<vector<int>>> interpret_results(vector<vector<int>> InitPositions, vector<vector<int>> TargetArray, vector<vector<int>> results){
        int n = results.size();
        int t = TargetArray.size();
        vector<vector<vector<int>>> moves = {};
        vector<int> moveto;
        vector<int> movefrom;
        for(int i = 0; i<n;i++){
            if(i<t && TargetArray[results[i][0]] != InitPositions[results[i][1]]){
                moveto = TargetArray[results[i][0]];
                movefrom = InitPositions[results[i][1]];
                moves.push_back({movefrom, moveto});
        }
}
    return moves;
}

vector<vector<vector<int>>> order(vector<vector<vector<int>>> moves, vector<vector<bool>> Array){
        int i = 0;
        int n = moves.size();
        vector<int> todo = {};
        vector<int> todo1 = {};
        int m;
        vector<vector<vector<int>>> properorder = {};
        while(i < n){
            if(Array[moves[i][1][0]][moves[i][1][1]] == false){
                Array = Move(Array,moves[i][0],moves[i][1]);
                properorder.push_back(moves[i]);
            }else{
                todo.push_back(i);
            }
            i ++;
        }
        int j;
        int k;
        while(1){
            todo1 = todo;
            todo = {};
            m = todo1.size();
            for(i = 0;i<m;i++){
                j = todo1[i];
                if(Array[moves[j][1][0]][moves[j][1][1]] == false){
                    Array = Move(Array,moves[j][0],moves[j][1]);
                    properorder.push_back(moves[j]);
                }else{
                    todo.push_back(j);
                }
            }
            k = todo.size();
            if(k == 0){
                break;
            }
        }
    return properorder;
}

int minfind(vector<int> a){
            int n = a.size();
            int minval = a[0];
            for(int i = 0; i < n;i++){

                if(a[i]<minval){
                    minval = a[i];
                }
            }
            return minval;
            }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step1(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
            int minval;
            for(int i = 0;i<n;i++){
                minval = minfind(CostMatrix[i]);
                for(int j = 0; j < n;j++){
                    CostMatrix[i][j] -= minval;
                }
            }
        return make_tuple(2,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step2(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
        for(int i = 0; i<n; i++){
            for(int j = 0; j<n;j++){
                if(CostMatrix[i][j] == 0 && col_covered[j] == false && row_covered[i] == false){
                    marked[i][j] = 1;
                    col_covered[j] = true;
                    row_covered[i] = true;
                    break;
                }
            }
        }


        tie(row_covered,col_covered) = clear_covers(n);
        return make_tuple(3,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step3(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
        int step;
        int counter  = 0;
        for(int i = 0;i<n;i++){
            for(int j = 0;j<n;j++){
                if(marked[i][j] == 1 && col_covered[j] == false){
                    col_covered[j] = true;
                    counter += 1;
                }
            }
        }
        if(counter >= n){
            step = 0;
        }else{
            step = 4;
        }
        return make_tuple(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step4(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
        int step = 0;
        bool done = false;
        signed int row = -1;
        signed int col = -1;
        signed int star_col = -1;
        while(done == false){
            tie(row,col) = find_zero(n, CostMatrix,row_covered,col_covered);
            if(row<0){
                done = true;
                step = 6;
            }else{
                marked[row][col] = 2;
                star_col = find_star_row(marked, row, n);
                if(star_col >= 0){
                    col = star_col;
                    row_covered[row] = true;
                    col_covered[col] = false;
                }else{
                    done = true;
                    Z0_r = row;
                    Z0_c = col;
                    step = 5;
                }
            }

        }
        return make_tuple(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step5(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
        int counter = 0;
        path[counter][0] = Z0_r;
        path[counter][1] = Z0_c;
        signed int row;
        signed int col;
        bool done = false;
        while(done == false){
            row = find_star_col(marked,path[counter][1], n);
            if(row>= 0){
                counter += 1;
                path[counter][0] = row;
                path[counter][1] = path[counter - 1][1];
            }else{
                done = true;
            }
            if(done == false){
                col = find_prime_row(n, path[counter][0], marked);
                counter += 1;
                path[counter][0] = path[counter-1][0];
                path[counter][1] = col;
            }
        }
        marked = convert_path(counter, path, n, marked);
        tie(row_covered, col_covered) = clear_covers(n);
        marked = erase_primes(n, marked);
        return make_tuple(3,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step6(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
        int minval = find_smallest(CostMatrix, n, row_covered, col_covered);
        for(int i = 0; i<n; i++){
            for(int j = 0; j<n; j++){
                if(row_covered[i] == true){
                    CostMatrix[i][j] += minval;
                }
                if(col_covered[j] == false){
                    CostMatrix[i][j] -= minval;
                }
            }
        }
        return make_tuple(4,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
}

tuple<vector<bool>,vector<int>,vector<vector<bool>>,vector<vector<bool>>> compute(vector<vector<bool>> Matrix){
            int ArrayDim = Matrix.size();
            int TargetDim;
            int atoms = 0;

                for(int i = 0;i<ArrayDim;i++){
                    for(int j = 0;j<ArrayDim;j++){
                        if(Matrix[i][j] == true){
                            atoms ++;
                        }
                    }
                }
            TargetDim = sqrt(atoms);

            vector<float> COM = HCenterOfMass(Matrix,ArrayDim);
            vector<vector<int>> TargetArray;
            vector<int> RowRange;
            vector<int> ColRange;
            tie(TargetArray,RowRange,ColRange) = ToTargetArray(TargetDim, COM);

            vector<vector<int>> CostMatrix;
            vector<bool> row_covered;
            vector<bool> col_covered;
            int n;
            int Z0_r = 0;
            int Z0_c = 0;
            vector<vector<int>> marked;
            vector<vector<int>> path;
            int diff;
            vector<vector<int>> InitPositions;
            tie(CostMatrix,InitPositions, diff) = ToCostMatrix(Matrix,TargetArray,ArrayDim);
            n = CostMatrix.size();
            tie(row_covered,col_covered) = clear_covers(n);
            int i = 0;
            vector<int> row;
            for(i = 0; i < n; i++){
                row.push_back(0);
            }
            for(i = 0; i < n; i++){
                marked.push_back(row);
            }
            row = {};
            for(i = 0; i < n*2; i++){
                row.push_back(0);
            }
            for(i = 0; i < n*2; i++){
                path.push_back(row);
            }
            bool done = false;
            int step = 1;
            vector<vector<int>> results = {};
            while(step != 0){
            switch(step){
                case 1: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step1(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 2: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step2(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 3: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step3(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 4: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step4(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 5: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step5(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 6: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step6(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
            }
            }

            for(i = 0; i < n; i++){
                for(int j = 0; j<n;j++){
                    if(marked[i][j] == 1){
                        results.push_back({i,j});
                    }
                }
            }

        vector<vector<vector<int>>> moves = interpret_results(InitPositions, TargetArray, results);
        vector<vector<vector<int>>> ordered_moves = order(moves, Matrix);

        vector<int> index;
        vector<bool> rowOrCol;
        vector<vector<bool>> oldRow;
        vector<vector<bool>> newRow;

        vector<vector<bool>> tempMatrix = Matrix;
        for(int i = 0; i<ordered_moves.size();i++){
            tempMatrix = Move(tempMatrix,ordered_moves[i][0],ordered_moves[i][1]);
           // cout << "{" << ordered_moves[i][0][0] << "," << ordered_moves[i][0][1] << "}->{" << ordered_moves[i][1][0] << "," << ordered_moves[i][1][1] << "}" << endl;
        }

        printArray(tempMatrix);

        int moveNumber = ordered_moves.size();
        for(i = 0;i<moveNumber;i++){
            if(ordered_moves[i][0][0] == ordered_moves[i][1][0]){
                rowOrCol.push_back(true);
                index.push_back(ordered_moves[i][0][0]);
                cout << "index:" << ordered_moves[i][0][0] << endl;
                oldRow.push_back(Matrix[ordered_moves[i][0][0]]);

                cout << "starting config" << endl;
                printArray({Matrix[ordered_moves[i][0][0]]});
                cout << "{" << ordered_moves[i][0][0] << "," << ordered_moves[i][0][1] << "}->{" << ordered_moves[i][1][0] << "," << ordered_moves[i][1][1] << "}" << endl;


                Matrix = Move(Matrix,ordered_moves[i][0],ordered_moves[i][1]);

                printArray(Matrix);

                newRow.push_back(Matrix[ordered_moves[i][0][0]]);

                cout << "ending config" << endl;
                printArray({Matrix[ordered_moves[i][0][0]]});

            }
            if(ordered_moves[i][0][1] == ordered_moves[i][1][1]){
                rowOrCol.push_back(false);
                index.push_back(moves[i][0][1]);
                Matrix = Transpose(Matrix);
                oldRow.push_back(Matrix[ordered_moves[i][0][1]]);
                cout << "index: " << ordered_moves[i][0][1] << endl;
                cout << "starting config" << endl;
                printArray({Matrix[ordered_moves[i][0][1]]});
                cout << "{" << ordered_moves[i][0][0] << "," << ordered_moves[i][0][1] << "}->{" << ordered_moves[i][1][0] << "," << ordered_moves[i][1][1] << "}" << endl;

                Matrix = Transpose(Matrix);
                Matrix = Move(Matrix,ordered_moves[i][0],ordered_moves[i][1]);
                printArray(Matrix);

                Matrix = Transpose(Matrix);
                newRow.push_back(Matrix[ordered_moves[i][0][1]]);

                cout << "ending config" << endl;
                printArray({Matrix[ordered_moves[i][0][1]]});

                Matrix = Transpose(Matrix);
            }
            if(ordered_moves[i][0][1] != ordered_moves[i][1][1] && ordered_moves[i][0][0] != ordered_moves[i][1][0]){
                //diagonal move to be split up
                cout << "diag" <<endl;
                //col move
                rowOrCol.push_back(false);
                index.push_back(moves[i][0][1]);
                Matrix = Transpose(Matrix);
                oldRow.push_back(Matrix[ordered_moves[i][0][1]]);
                Matrix = Transpose(Matrix);
                Matrix = Move(Matrix,ordered_moves[i][0],{ordered_moves[i][0][0],ordered_moves[i][1][1]});
                Matrix = Transpose(Matrix);
                newRow.push_back(Transpose(Matrix)[ordered_moves[i][0][1]]);
                Matrix = Transpose(Matrix);

                // row move
                rowOrCol.push_back(true);
                index.push_back(moves[i][0][0]);
                oldRow.push_back(Matrix[ordered_moves[i][0][0]]);
                Matrix = Move(Matrix,{ordered_moves[i][0][0],ordered_moves[i][1][1]},ordered_moves[i][1]);
                newRow.push_back(Matrix[ordered_moves[i][0][0]]);
            }
        }
        return make_tuple(rowOrCol,index,oldRow,newRow);
}
////////////////////////////SNAKE//////////////////////////////




////////////////////////////END//////////////////////////////

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
    vector<RearrangementMove> moves;

    if(method == BALANCE_COMPRESS){
        tie(initCols,initOldCols,initNewCols,rows,oldRows,newRows,cols,oldCols,newCols) = BalanceCompressAlg(Array);

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

    }
    if(method == RECT_BALANCE_COMPRESS){
        tie(rows,oldRows,newRows,cols,oldCols,newCols) = RBalanceCompress(Array);
        initCols.clear();

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

    }

    if(method == HUNGARIAN){
        vector<bool> rowOrCol;
        vector<int> index;
        vector<vector<bool>> oldRow;
        vector<vector<bool>> newRow;
        tie(rowOrCol,index,oldRow,newRow) = compute(Array);

        for(int i = 0;i<rowOrCol.size();i++){
            moves.push_back(RearrangementMove());
            moves[i].startingConfig = oldRow[i];
            moves[i].endingConfig = newRow[i];
            moves[i].row = rowOrCol[i]; // 1 if row, 0 if col
            moves[i].dim = index[i];
        }
    }

    return moves;
}


vector<vector<bool>> MakeBoolArray(int Dim, float Prob){
    int i = 0;
    int j = 0;
    vector<vector<bool>> matrix;
    vector<bool> row;
    srand(time(NULL));
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

tuple<int,int> ArrayDimensions(int sites, float prob){
    float H = sqrt(prob*sites);
    int Height = H;
    float W = sites/Height;
    int Width = W;

    return make_tuple(Height,Width);
}

vector<vector<bool>> MakeRectArray(int Sites,float Prob){
    int i = 0;
    int j = 0;
    int Width;
    int Height;
    tie(Height, Width) = ArrayDimensions(Sites,Prob);
    vector<vector<bool>> matrix;
    vector<bool> row;
    while (i < Height){
        while(j < Width){
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
