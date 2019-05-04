#ifndef REARRANGE2D_NEW_H
#define REARRANGE2D_NEW_H

#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <ctime>
#include <math.h>
#include "rearrangementmove.h"

using namespace std;
class Rearrange2d_new{
    public:
        vector<RearrangementMove> rearrange(vector<vector<bool>> Array, enum rearrange_method method,int mode = 0);

    private:
        int i;
        int j;
        int k;
        int COM[2];
        vector<bool> ColumnAt(vector<vector<bool>> Array, int dim);
        vector<bool> CompressRow(vector<bool> row, int left, int right, int atoms);
        vector<int> RowSum(vector<vector<bool>> Array, int dimension);
        tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>,vector<RearrangementMove>> Balance(vector<vector<bool>> Array, vector<int> Range, int dim, int SufficientAtoms, vector<int> RowTotal);
        vector<vector<bool>> assignCol(vector<vector<bool>> Array, vector<bool> col, int index);
        int* CenterOfMass(vector<vector<bool>> Array, int dim);
        vector<RearrangementMove> BalanceCompressAlg(vector<vector<bool>> Array, int mode);

};


#endif // REARRANGE2D_NEW_H
