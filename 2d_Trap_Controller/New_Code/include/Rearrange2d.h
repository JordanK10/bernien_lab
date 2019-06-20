#ifndef REARRANGE2D_H
#define REARRANGE2D_H

#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <ctime>
#include <math.h>
#include <chrono>


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
  HUNGARIAN,
  DROP_IT_LIKE_ITS_HOT
};

using namespace std;

struct RearrangementMove {
	std::vector<bool> startingConfig;
	std::vector<bool> endingConfig;
	bool row; // 1 if row, 0 if col
	int dim; // the endex of the desired move
};

//Common Functions
void printArray(vector<vector<bool>> Array);
vector<RearrangementMove> rearrange(vector<vector<bool>> &Array, rearrange_method method,rearrange_mode mode = CENTER_COM);
vector<vector<bool>> assignCol(vector<vector<bool>> Array, vector<bool> col, int index);
vector<bool> ColumnAt(vector<vector<bool>> Array, int dim);
int* CenterOfMass(vector<vector<bool>> Array);

//Balance-Compress Functions
bool eq(vector<bool> starting, vector<bool> ending);
vector<bool> CompressRow(vector<bool> row, int left, int right, int atoms);
vector<int> RowSum(vector<vector<bool>> Array);
vector<vector<int>> Balance(vector<vector<bool>> &Array, vector<int> &Range, int SufficientAtoms, vector<int> &RowTotal);
vector<RearrangementMove> BalanceCompressAlg(vector<vector<bool>> &Array, int mode);

//Hungarian Functions
void clear_covers(vector<bool> &a, vector<bool>&b);
void find_zero(vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered,signed int &row,signed int &col);
int find_star_row(vector<vector<int>> marked, int row);
int find_star_col(vector<vector<int>> marked, int col);
int find_prime_row(int row, vector<vector<int>> marked);
void erase_primes(vector<vector<int>> &marked);
void convert_path(int counter, vector<vector<int>> path, vector<vector<int>> &marked);
int find_smallest(vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered);
int metric(vector<int> pos1, vector<int> pos2,int dim);
void ToCostMatrix(vector<vector<bool>> InitArray, vector<vector<int>> TargetArray, int Dim1,vector<vector<int>> &InitPositions,vector<vector<int>> &CostMatrix);
vector<vector<int>> ToTargetArray(int dim, int r0,int r1, int c0 ,int c1);
vector<vector<vector<int>>> interpret_results(vector<vector<int>> InitPositions, vector<vector<int>> TargetArray, vector<vector<int>> results);
vector<vector<vector<int>>> order(vector<vector<vector<int>>> moves, vector<vector<bool>> Array);
int minfind(vector<int> a);
int step1(vector<vector<int>> &CostMatrix);
int step2(vector<vector<int>> &CostMatrix, vector<bool> &row_covered, vector<bool> &col_covered, vector<vector<int>> &marked);
int step3(vector<vector<int>> &CostMatrix, vector<bool> &col_covered, vector<vector<int>> &marked);
int step4(vector<vector<int>> &CostMatrix, vector<bool> &row_covered, vector<bool> &col_covered, vector<vector<int>> &marked, int &Z0_c, int &Z0_r);
int step5(vector<bool> &row_covered, vector<bool> &col_covered, vector<vector<int>> &marked, vector<vector<int>> &path, int &Z0_c, int &Z0_r);
int step6(vector<vector<int>> &CostMatrix, vector<bool> &row_covered, vector<bool> &col_covered);
vector<RearrangementMove> compute(vector<vector<bool>> &Matrix,rearrange_mode mode);

//Atom Drop Functions
void go(int offset, int k,vector<vector<int>> &r,vector<int> &setOfVals,vector<int> &combination);
vector<vector<int>> choose(vector<int> start, int n);
bool test(vector<vector<bool>> Array,vector<int> &rows,vector<int> &cols, vector<int> rowTotals, vector<int> colTotals,int n);
bool check(vector<vector<bool>> Array,vector<int> rows, vector<int> cols);
bool in(vector<int> row, int n);
vector<RearrangementMove> DropItLikeItsHot(vector<vector<bool>> &Array);

//Banking and Replenishing
bool detectVacancies(vector<vector<int>> &vacant,vector<vector<bool>> Array);
void findExtras(vector<vector<bool>> Array, vector<vector<int>> &atoms);
vector<RearrangementMove> bank(vector<vector<bool>> &Array);
int findNearestBank(vector<vector<bool>> Array, vector<int> vacancy);
vector<RearrangementMove> fillVacancies(vector<vector<bool>> &Array);
vector<RearrangementMove> rectBank(vector<vector<bool>> &Array);
bool Toggle(bool a);
#endif // REARRANGE2D_H
