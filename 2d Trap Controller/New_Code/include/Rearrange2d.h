#ifndef REARRANGE2D_NEW_H
#define REARRANGE2D_NEW_H

#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <ctime>
#include <math.h>


enum rearrange_mode {
	CENTER_COM,
	UL_CORNER,
	UR_CORNER,
	LL_CORNER,
	LR_CORNER,
    CLOSE_CORNER,
	REC_LEFT,
	REC_RIGHT,
	REC_CENT,
};

enum rearrange_method {
  BALANCE_COMPRESS,
  HUNGARIAN
};



using namespace std;

struct RearrangementMove {
	std::vector<bool> startingConfig;
	std::vector<bool> endingConfig;
	bool row; // 1 if row, 0 if col
	int dim; // the endex of the desired move
};



vector<RearrangementMove> rearrange(vector<vector<bool>> Array, rearrange_method method,rearrange_mode mode = CENTER_COM);
vector<vector<bool>> assignCol(vector<vector<bool>> Array, vector<bool> col, int index);


vector<bool> ColumnAt(vector<vector<bool>> Array, int dim);
vector<bool> CompressRow(vector<bool> row, int left, int right, int atoms);
vector<int> RowSum(vector<vector<bool>> Array);
vector<vector<int>> Balance(vector<vector<bool>> &Array, vector<int> &Range, int SufficientAtoms, vector<int> &RowTotal,vector<RearrangementMove> &moves);

int* CenterOfMass(vector<vector<bool>> Array);
vector<RearrangementMove> BalanceCompressAlg(vector<vector<bool>> Array, int mode);



#endif // REARRANGE2D_NEW_H
