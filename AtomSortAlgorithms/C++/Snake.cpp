#include "Snake.h"

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

Snake::Snake(int Dim, float Prob)
{
    Array = MakeBoolArray(Dim, Prob);
}

vector<vector<bool>> Snake::MakeBoolArray(int Dim, float Prob){
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

bool Snake::Toggle(bool a){
    bool b;
    if(a == true){
        b = false;
    }else{
        b = true;
    }
    return b;
}

vector<vector<int>> Snake::StdStart(int ArrayDim, int TargetDim, vector<vector<int>> path){
    int i = ArrayDim - TargetDim;
    int j = 0;
    bool invert = true;
    while(i < ArrayDim){
        while(j < ArrayDim && j>-1){
            path.push_back({j,i});
            if(invert == true){
                j ++;
            }else{
                j --;
            }
        }
        i++;
        if(invert == true){
            j --;
        }else{
            j ++;
        }
        invert = Toggle(invert);
    }
    return path;
}

vector<vector<int>> Snake::AltStart(int ArrayDim,int TargetDim,vector<vector<int>> path){
    int i = ArrayDim - 1;
    int j = TargetDim - 1;
    bool invert = false;
    while(i>ArrayDim - TargetDim - 1){
        while(j<TargetDim && j>-1){
            path.push_back({j,i});
            if(invert == true){
                j ++;
            }else{
                j --;
            }
        }
        i --;
        if(invert == true){
            j -= 1;
        }else{
            j ++;
        }
        invert = Toggle(invert);
    }
    return path;
}

vector<vector<int>> Snake::Down(int ArrayDim,int TargetDim, vector<vector<int>> path){
    int i = ArrayDim - 1;
    int j = TargetDim;
    bool invert = true;
    while(i>ArrayDim - TargetDim - 1){
        while(j < ArrayDim && j > TargetDim - 1){
            path.push_back({j,i});
            if(invert == true){
                j ++;
            }else{
                j --;
            }
        }
        i --;
        if(invert == true){
            j--;
        }else{
            j ++;
        }
        invert = Toggle(invert);
    }
    return path;
}

vector<vector<int>> Snake::Left(int ArrayDim,int TargetDim, vector<vector<int>> path){
    int i = ArrayDim - 1;
    int j = TargetDim;
    bool invert = false;
    while(j<ArrayDim){
        while(i<ArrayDim && i>ArrayDim - TargetDim - 1){
            path.push_back({j,i});
            if(invert == true){
                i++;
            }else{
                i --;
            }
        }
        j ++;
        if(invert == true){
            i--;
        }else{
            i ++;
        }
        invert = Toggle(invert);
    }
    return path;
}

vector<vector<int>> Snake::Right(int ArrayDim,int TargetDim, vector<vector<int>> path){
    int i = ArrayDim - TargetDim;
    int j = TargetDim;
    bool invert = true;
    while(j<ArrayDim){
        while(i<ArrayDim && i>ArrayDim - TargetDim - 1){
            path.push_back({j,i});
            if(invert == true){
                i ++;
            }else{
                i --;
            }
        }
        j ++;
        if(invert == true){
            i --;
        }else{
            i ++;
        }
        invert = Toggle(invert);
        }
        return path;
}

vector<vector<int>> Snake::Finish(int ArrayDim, int TargetDim, vector<vector<int>> path){
    int i = ArrayDim - TargetDim - 1;
    int j = ArrayDim - 1;
    bool invert = false;
    while(i>-1){
        while(j>-1 && j<ArrayDim){
            path.push_back({j,i});
            if(invert == true){
                j ++;
            }else{
                j--;
            }
        }
        i --;
        if(invert == true){
            j --;
        }else{
            j ++;
        }
        invert = Toggle(invert);
    }
    return path;
}

vector<vector<int>> Snake::MakePath(int ArrayDim, int TargetDim){
    vector<vector<int>> path = {};
    int diff = ArrayDim - TargetDim;
    if(diff%2 == 1){
        path = StdStart(ArrayDim,TargetDim,path);
        path = Left(ArrayDim, TargetDim, path);
    }else{
        if(ArrayDim%2 == 0){
            path = AltStart(ArrayDim, TargetDim, path);
            path = Right(ArrayDim, TargetDim, path);
        }else{
            path = StdStart(ArrayDim,TargetDim, path);
            path = Down(ArrayDim, TargetDim, path);
        }
    }
    path = Finish(ArrayDim, TargetDim, path);
    return path;
}

vector<vector<bool>> Snake::Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2){
    vector<vector<bool>> NewArray = Array;
    NewArray[pos1[0]][pos1[1]] = false;
    NewArray[pos2[0]][pos2[1]] = true;
    return NewArray;
}

bool Snake::Check(vector<vector<bool>> Array, vector<int> RowRange, vector<int> ColRange){
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

tuple<vector<vector<bool>>, vector<vector<vector<int>>>> Snake::Advance(vector<vector<bool>> Array, int i, vector<vector<int>> path, vector<vector<vector<int>>> moves){
    while(i>0){
        if(Array[path[i-1][0]][path[i-1][1]] == true){
            break;
        }else{
            Array = Move(Array,path[i],path[i-1]);
            moves.push_back({path[i],path[i-1]});
            i -= 1;
        }
    }
    return make_tuple(Array,moves);
}

tuple<float, vector<vector<vector<int>>>, bool> Snake::SnakeAlg(vector<vector<bool>> Array, int TargetDim){
int ArrayDim = Array.size();
int i = 0;
int j = 0;
int atoms = 0;
if(TargetDim == 0){
    for(i = 0;i<ArrayDim;i++){
        for(j = 0;j<ArrayDim;j++){
            if(Array[i][j] == true){
                atoms ++;
            }
        }
    }
float x = sqrt(atoms);
TargetDim = x;
}

vector<vector<int>> path = MakePath(ArrayDim,TargetDim);

auto start = chrono::steady_clock::now();

vector<vector<vector<int>>> moves = {};
i = 0;
j = 0;

while(j<path.size()){
    if(Array[path[j][0]][path[j][1]] == true){
        Array = Move(Array,path[j],path[i]);
        moves.push_back({path[j],path[i]});
        i ++;
    }
    j ++;
}

/*
while(i<path.size()){
    if(Array[path[i][0]][path[i][1]] == true){
        tie(Array,moves) = Advance(Array,i,path,moves);
    }
    i ++;
}
*/
vector<int> RowRange = {0, TargetDim - 1};
vector<int> ColRange = {ArrayDim - TargetDim,ArrayDim - 1};

auto End = chrono::steady_clock::now();
float duration = chrono::duration_cast<chrono::microseconds>(End - start).count();
bool check = Check(Array,RowRange,ColRange);
return make_tuple(duration,moves,check);
}
