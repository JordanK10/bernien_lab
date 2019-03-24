#ifndef RECTBC_H
#define RECTBC_H

#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <cmath>
#include <tuple>
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <string>
#include <math.h>

using namespace std;

class RectBC
{
public:

vector<vector<bool>> MakeRectArray(int Sites, float Prob){
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

tuple<int,int> ArrayDimensions(int sites, float prob){
    float H = sqrt(prob*sites);
    int Height = H;
    float W = sites/Height;
    int Width = W;

    return make_tuple(Height,Width);
}

vector<int> CenterOfMass(vector<vector<bool>> Array, int Height, int Width){
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
        k ++;
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

vector<int> RowSum(vector<vector<bool>> Array, int H, int W){
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

tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<vector<vector<int>>>> Balance(vector<vector<bool>> Array, vector<int> Range, vector<int> COM, int Height,int Width, vector<int> RowTotal){
    vector<vector<vector<int>>> moves;
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
    moves.push_back({movefrom,moveto});
    RowTotal[movefrom[0]] --;
    RowTotal[moveto[0]] ++;
    }
}
vector<int> Range1 = {center + 1,Range[1]};
vector<int> Range0 = {Range[0],center};
return make_tuple(Array,RowTotal,Range0,Range1,moves);
}

tuple<float, vector<vector<vector<int>>>,vector<int>,vector<vector<vector<int>>>, bool> BalanceCompress(vector<vector<bool>> Array){
    auto start = chrono::steady_clock::now();

    vector<vector<vector<int>>> moves2;
    int Height = Array.size();
    int Width = Array[0].size();

    vector<int> COM = CenterOfMass(Array, Height, Width);
    vector<int> RowTotals = RowSum(Array, Height, Width);
    int atoms  = 0;
    bool check1 = true;
    for(int y = 0;y<Height;y++){
        if(RowTotals[y]<Height){
            check1 = false;
        }
        atoms += RowTotals[y];
    }
    cout << atoms << endl;

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
    int k = 0;

    while(BalancedRows.size() < Height && check1 == false){
    tie(Array,RowTotals,Range1,Range2,newmoves) = Balance(Array,RowRange[i],COM,Height,Width,RowTotals);

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
    for(int l = 0;l<Height;l++){
        rowmoves.push_back({{0,0}});
    }
    while(i <= RowRange[0][1]){
        tie(Array[i],rowmoves[i]) = CompressRow(Array[i],COM[1],Height);
        rows.push_back(i);
        i++;
    }

auto End = chrono::steady_clock::now();

vector<vector<vector<int>>> BankMoves = Bank(Array,ColRange[0]);
for(i = 0;i<BankMoves.size();i++){
   moves2.push_back(BankMoves[i]);
}

for(i = 0; i<BankMoves.size(); i++){
    Array = Move(Array,BankMoves[i][0],BankMoves[i][1]);
}

for(int g = 0;g<Height;g++){
    for(int h = 0;h<Width;h++){
        cout << Array[g][h] << " ";
    }
    cout << endl;
}
cout << endl;

float duration = chrono::duration_cast<chrono::microseconds>(End - start).count();
bool check = Check(Array,RowRange[0],ColRange);
return make_tuple(duration,moves2,rows,rowmoves,check);
}
};
#endif // RECTBC_H
