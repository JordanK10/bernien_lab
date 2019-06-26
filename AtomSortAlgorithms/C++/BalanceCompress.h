#ifndef BALANCECOMPRESS_H
#define BALANCECOMPRESS_H

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

class BalanceCompress
{
    public:
        BalanceCompress(int Dim, float Prob);
        vector<vector<bool>> MakeBoolArray(int Dim, float Prob);
        tuple<float, vector<vector<vector<int>>>,vector<int>,vector<vector<vector<int>>>, bool> BalanceCompressAlg(vector<vector<bool>> Array, int TargetDim = 0);
        vector<vector<bool>> Array;

    private:
        vector<int> CenterOfMass(vector<vector<bool>> Array, int Dim);
        tuple<vector<bool>,vector<vector<int>>> CompressRow(vector<bool> row, int center, int suff);
        vector<vector<bool>> Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2);
        vector<int> RowSum(vector<vector<bool>> Array, int dim);
        vector<vector<bool>> Transpose(vector<vector<bool>> Array, int dim);
        bool Check(vector<vector<bool>> Array, vector<int> RowRange, vector<int> ColRange);
        tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<vector<vector<int>>>> Balance(vector<vector<bool>> Array, vector<int> Range, vector<int> COM, int dim, int SufficientAtoms, vector<int> RowTotal);
        vector<vector<vector<int>>> Interpolate(vector<int> movefrom, vector<int> moveto, int center);
};

#endif // BALANCECOMPRESS_H
