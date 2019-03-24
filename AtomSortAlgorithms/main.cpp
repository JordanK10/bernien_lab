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
#include "BalanceCompressAlgorithm.h"
#include "Hungarian.h"
#include "RectBC.h"
#include "Snake.h"

using namespace std;

int main()
{
int Algorithm = 4;
int ArrayDim = 10;
int TargetDim = 6;
int Sites = 100;


BalanceCompressAlgorithm obj1;
Hungarian obj2;
RectBC obj3;
Snake obj4;


float duration;
bool check;
vector<vector<vector<int>>> results;
vector<vector<vector<int>>> moves;
vector<int> rows;
vector<vector<vector<int>>> rowmoves;

vector<vector<bool>> Array = obj1.MakeBoolArray(ArrayDim,.6);
vector<vector<bool>> RArray = obj3.MakeRectArray(Sites,.6);

if(Algorithm == 1){
    tie(duration, moves,rows, check) = obj1.BalanceCompress(Array, ArrayDim, TargetDim);
}
//if(Algorithm == 2){
//    obj2.compute(Array, TargetDim);
//}
if(Algorithm == 3){
    tie(duration,moves,rows,rowmoves,check) = obj3.BalanceCompress(RArray);
}
if(Algorithm == 4){
    tie(duration ,moves, check) = obj4.SnakeAlg(Array, ArrayDim, TargetDim);
}


cout << "Fidelity: " << check << endl;
cout << "Duration: " << duration << endl;
cout << moves.size() << " Moves:" << endl;
int n = moves.size();
for(int i = 0;i<n;i++){
    cout << "(" << moves[i][0][0] << ", " << moves[i][0][1] << ") -> (" << moves[i][1][0] << ", " << moves[i][1][1] << ")" << endl;
}
if(Algorithm == 1 || Algorithm == 3){
    n = rows.size();
    for(int i = 0; i<n; i++){
        cout << "Compress Row: " << rows[i] << endl;
    }
}
}
