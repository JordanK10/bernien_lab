#ifndef RECTBC_H
#define RECTBC_H

#include <vector>
#include <tuple>
#include <random>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;


class RectBC
{
    public:
        RectBC();
        tuple<vector<vector<bool>>, vector<vector<vector<int>>>, vector<vector<int>>, vector<int> , vector<int>,bool> BalanceCompress(vector<vector<bool>> Array);
        double moveFidelity;
        vector<vector<bool>> MakeRectArray(int Sites, float Prob);

    private:

        tuple<int,int> ArrayDimensions(int sites, float prob);
        vector<int> CenterOfMass(vector<vector<bool>> Array, int Height, int Width);
        bool Toggle(bool a);
        vector<vector<vector<int>>> Bank(vector<vector<bool>> Array, int Range);
        tuple<vector<bool>,vector<vector<int>>> CompressRow(vector<bool> row, int center, int suff);
        vector<vector<bool>> Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2);
        vector<int> RowSum(vector<vector<bool>> Array, int H, int W);
        vector<vector<bool>> Transpose(vector<vector<bool>> Array, int dim);
        bool Check(vector<vector<bool>> Array, vector<int> RowRange, vector<int> ColRange);
        tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<vector<vector<int>>>> Balance(vector<vector<bool>> Array, vector<int> Range, vector<int> COM, int Height,int Width, vector<int> RowTotal);
        vector<vector<vector<int>>> Interpolate(vector<int> movefrom, vector<int> moveto, int center);
};

#endif // RECTBC_H
