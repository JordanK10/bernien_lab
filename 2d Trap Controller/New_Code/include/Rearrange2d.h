#ifndef BALANCECOMPRESS_H
#define BALANCECOMPRESS_H

#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <ctime>
#include <math.h>
#include "rearrangementmove.h"

using namespace std;

class BalanceCompress
{
    public:
        vector<RearrangementMove> rearrange(vector<vector<bool>> Array, enum rearrange_method method);
        vector<vector<bool>> MakeBoolArray(int Dim, float Prob);
        vector<vector<bool>> MakeRectArray(int Sites,float Prob);


    private:
        vector<vector<vector<int>>> Interpolate(vector<int> movefrom, vector<int> moveto, int center);
        vector<int> CenterOfMass(vector<vector<bool>> Array, int Dim);
        vector<bool> CompressRow(vector<bool> row, int center, int suff);
        vector<vector<bool>> Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2);
        vector<int> RowSum(vector<vector<bool>> Array, int dim);
        vector<vector<bool>> Transpose(vector<vector<bool>> Array);
        tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<vector<vector<int>>>,vector<int>,vector<vector<bool>>,vector<vector<bool>>> Balance(vector<vector<bool>> Array, vector<int> Range, vector<int> COM, int dim, int SufficientAtoms, vector<int> RowTotal);
        tuple<vector<int>,vector<vector<bool>>, vector<vector<bool>>,vector<int>,vector<vector<bool>>, vector<vector<bool>>,vector<int>,vector<vector<bool>>, vector<vector<bool>>> BalanceCompressAlg(vector<vector<bool>> Array);
        vector<int> RCenterOfMass(vector<vector<bool>> Array, int Height, int Width);
        bool Toggle(bool a);
        vector<vector<vector<int>>> Bank(vector<vector<bool>> Array, int Range);
        vector<int> RRowSum(vector<vector<bool>> Array, int H, int W);
        tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<vector<vector<int>>>,vector<int>, vector<vector<bool>>, vector<vector<bool>>> RBalance(vector<vector<bool>> Array, vector<int> Range, vector<int> COM, int Height,int Width, vector<int> RowTotal);
        tuple<vector<int>,vector<vector<bool>>,vector<vector<bool>>,vector<int>,vector<vector<bool>>,vector<vector<bool>>> RBalanceCompress(vector<vector<bool>> Array);
        tuple<vector<bool>,vector<bool>> clear_covers(int n);
        bool Check(vector<vector<bool>> Array, vector<int> RowRange, vector<int> ColRange);
        tuple<int,int> find_zero(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered);
        int find_star_row(vector<vector<int>> marked, int row, int n);
        int find_star_col(vector<vector<int>> marked, int col, int n);
        int find_prime_row(int n, int row, vector<vector<int>> marked);
        vector<vector<int>> erase_primes(int n, vector<vector<int>> marked);
        vector<vector<int>> convert_path(int counter, vector<vector<int>> path, int n, vector<vector<int>> marked);
        int find_smallest(vector<vector<int>> CostMatrix, int n, vector<bool> row_covered, vector<bool> col_covered);
        int metric(vector<int> pos1, vector<int> pos2,int dim);
        tuple<vector<vector<int>>,vector<vector<int>>, int> ToCostMatrix(vector<vector<bool>> InitArray, vector<vector<int>> TargetArray, int Dim1);
        tuple<vector<vector<int>>, vector<int>, vector<int>> ToTargetArray(int dim, vector<float> COM);
        vector<float> HCenterOfMass(vector<vector<bool>> Array, int dim);
        vector<vector<vector<int>>> interpret_results(vector<vector<int>> InitPositions, vector<vector<int>> TargetArray, vector<vector<int>> results);
        vector<vector<vector<int>>> order(vector<vector<vector<int>>> moves, vector<vector<bool>> Array);
        int minfind(vector<int> a);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step1(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step2(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step3(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step4(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step5(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step6(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<vector<bool>,vector<int>,vector<vector<bool>>,vector<vector<bool>>> compute(vector<vector<bool>> Matrix);
        tuple<int,int> ArrayDimensions(int sites, float prob);

};

#endif // BALANCECOMPRESS_H
