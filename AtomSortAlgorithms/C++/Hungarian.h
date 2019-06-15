#ifndef HUNGARIAN_H
#define HUNGARIAN_H

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

class Hungarian
{
    public:
        Hungarian(int dim, float prob);
        vector<vector<bool>> Array;
        tuple<float, vector<vector<vector<int>>>, bool> compute(vector<vector<bool>> Matrix,int TargetDim = 0);

    private:
        vector<vector<bool>> MakeBoolArray(int Dim, float Prob);
        bool Check(vector<vector<bool>> Array, vector<int> RowRange, vector<int> ColRange);
        tuple<vector<bool>,vector<bool>> clear_covers(int n);
        vector<vector<bool>> Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2);
        tuple<int,int> find_zero(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered);
        int find_star_row(vector<vector<int>> marked, int row, int n);
        int find_star_col(vector<vector<int>> marked, int col, int n);
        int find_prime_row(int n, int row, vector<vector<int>> marked);
        vector<vector<int>> erase_primes(int n, vector<vector<int>> marked);
        vector<vector<int>> convert_path(int counter, vector<vector<int>> path, int n, vector<vector<int>> marked);
        int find_smallest(vector<vector<int>> CostMatrix, int n, vector<bool> row_covered, vector<bool> col_covered);
        int metric(vector<int> pos1, vector<int> pos2);
        tuple<vector<vector<int>>,vector<vector<int>>, int> ToCostMatrix(vector<vector<bool>> InitArray, vector<vector<int>> TargetArray, int Dim1);
        tuple<vector<vector<int>>, vector<int>, vector<int>> ToTargetArray(int dim, vector<float> COM);
        vector<float> CenterOfMass(vector<vector<bool>> Array, int dim);
        vector<vector<vector<int>>> interpret_results(vector<vector<int>> InitPositions, vector<vector<int>> TargetArray, vector<vector<int>> results);
        vector<vector<vector<int>>> order(vector<vector<vector<int>>> moves, vector<vector<bool>> Array);
        int minfind(vector<int> a);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step1(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step2(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step3(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step4(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step5(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);
        tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> step6(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r);

};

#endif // HUNGARIAN_H
