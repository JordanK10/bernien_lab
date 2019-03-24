#ifndef BALANCECOMPRESSALGORITHM_H
#define BALANCECOMPRESSALGORITHM_H


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

using namespace std;
class BalanceCompressAlgorithm{
    public:
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

vector<bool> CompressRow(int center, int dim, int rowtotal){
    int i = 0;
    float a = rowtotal;
    vector<bool> NewRow;
    float start = center - a/2;
    float finish = center + a/2;
    signed int Start = start;
    int Finish = finish;

    if(Start <= 0){
        Start = 0;
        Finish = rowtotal - 1;
    }
    if(Finish >= dim - 1){
        Finish = dim - 1;
        Start = Finish - rowtotal;
    }
    if(Start > 0 && Finish < dim -1){
        while(i<Start){
            NewRow.push_back(false);
            i ++;
        }
        while(i <= Finish){
            NewRow.push_back(true);
            i ++;
        }
        while(i < dim){
            NewRow.push_back(false);
            i ++;
        }
    }
    return NewRow;
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
    moves.push_back({movefrom,moveto});
    RowTotal[movefrom[0]] --;
    RowTotal[moveto[0]] ++;
    }
}
vector<int> Range1 = {center + 1,Range[1]};
vector<int> Range0 = {Range[0],center};
return make_tuple(Array,RowTotal,Range0,Range1,moves);
}

tuple<float, vector<vector<vector<int>>>,vector<int>, bool> BalanceCompress(vector<vector<bool>> Array, int ArrayDim, int TargetDim = 0){
    vector<vector<vector<int>>> moves2;
    auto start = chrono::steady_clock::now();

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

    cout << 2 << endl;

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
    while(i <= RowRange[0][1]){
        Array[i] = CompressRow(COM[1],ArrayDim,RowTotals[i]);
        rows.push_back(i);
        i++;
    }

auto End = chrono::steady_clock::now();
float duration = chrono::duration_cast<chrono::microseconds>(End - start).count();
bool check = Check(Array,RowRange[0],ColRange);
return make_tuple(duration,moves2,rows,check);
}
};

#endif // BALANCECOMPRESSALGORITHM_H
