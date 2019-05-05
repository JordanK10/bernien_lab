#ifndef REARRANGE2D_NEW_H
#define REARRANGE2D_NEW_H

#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <ctime>
#include <math.h>

using namespace std;

struct RearrangementMove {
	std::vector<bool> startingConfig;
	std::vector<bool> endingConfig;
	bool row; // 1 if row, 0 if col
	int dim; // the endex of the desired move
};

enum rearrange_method{
    HUNGARIAN,
    BALANCE_COMPRESS,
    RECT_BALANCE_COMPRESS
};

class Rearrange2d_new{
    public:
        vector<RearrangementMove> rearrange(vector<vector<bool>> Array, enum rearrange_method method,int mode = 0);
        vector<vector<bool>> assignCol(vector<vector<bool>> Array, vector<bool> col, int index);

    private:
        vector<bool> ColumnAt(vector<vector<bool>> Array, int dim);
        vector<bool> CompressRow(vector<bool> row, int left, int right, int atoms);
        vector<int> RowSum(vector<vector<bool>> Array);
        tuple<vector<vector<bool>>,vector<int>,vector<int>,vector<int>> Balance(vector<vector<bool>> Array, vector<int> Range, int SufficientAtoms, vector<int> RowTotal);

        int* CenterOfMass(vector<vector<bool>> Array);
        vector<RearrangementMove> BalanceCompressAlg(vector<vector<bool>> Array, int mode);
};


#endif // REARRANGE2D_NEW_H
