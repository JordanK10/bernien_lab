#ifndef SNAKE_H
#define SNAKE_H

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

class Snake
{
    public:
        Snake(int Dim, float Prob);
        vector<vector<bool>> Array;
        tuple<float, vector<vector<vector<int>>>, bool> SnakeAlg(vector<vector<bool>> Array, int TargetDim = 0);

    private:
        vector<vector<bool>> MakeBoolArray(int Dim, float Prob);
        bool Toggle(bool a);
        vector<vector<int>> StdStart(int ArrayDim, int TargetDim, vector<vector<int>> path);
        vector<vector<int>> AltStart(int ArrayDim,int TargetDim,vector<vector<int>> path);
        vector<vector<int>> Down(int ArrayDim,int TargetDim, vector<vector<int>> path);
        vector<vector<int>> Left(int ArrayDim,int TargetDim, vector<vector<int>> path);
        vector<vector<int>> Right(int ArrayDim,int TargetDim, vector<vector<int>> path);
        vector<vector<int>> Finish(int ArrayDim, int TargetDim, vector<vector<int>> path);
        vector<vector<int>> MakePath(int ArrayDim, int TargetDim);
        vector<vector<bool>> Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2);
        bool Check(vector<vector<bool>> Array, vector<int> RowRange, vector<int> ColRange);
        tuple<vector<vector<bool>>, vector<vector<vector<int>>>> Advance(vector<vector<bool>> Array, int i, vector<vector<int>> path, vector<vector<vector<int>>> moves);



};

#endif // SNAKE_H
