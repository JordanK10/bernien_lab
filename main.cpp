#include <iostream>
#include <vector>
#include "RectBalanceCompress.h"
#include "BalanceCompress.h"
#include "Snake.h"
#include "Hungarian.h"

using namespace std;

int main()
{
/*
1 -> RectBC
2 -> BC
3 -> Snake
4 -> Hungarian
*/
int Algorithm = 2;
int ArrayDim = 10;
int TargetDim = 0;
int Sites = 100;
float LoadProbability = 0.6;

double duration;
bool check;
vector<vector<vector<int>>> moves;
vector<int> rows;
vector<vector<vector<int>>> rowmoves;
vector<vector<vector<int>>> bankmoves;

if(Algorithm == 1){
RectBalanceCompress RBC_Obj(Sites,LoadProbability);
tie(duration, moves, rows, rowmoves, bankmoves, check) = RBC_Obj.BalanceCompress(RBC_Obj.Array);
}
if(Algorithm == 2){
BalanceCompress BC_Obj(ArrayDim, LoadProbability);
tie(duration, moves,rows,rowmoves, check) = BC_Obj.BalanceCompressAlg(BC_Obj.Array);
}
if(Algorithm == 3){
Snake S_Obj(ArrayDim, LoadProbability);
tie(duration ,moves, check) = S_Obj.SnakeAlg(S_Obj.Array);
}
if(Algorithm == 4){
Hungarian H_Obj(ArrayDim, LoadProbability);
tie(duration, moves, check) = H_Obj.compute(H_Obj.Array);
}


cout << "Fidelity: " << check << endl;
cout << "Duration: " << duration << " nanoseconds" <<endl;
cout << moves.size() + rows.size() + bankmoves.size() << " Moves" << endl;
cout << endl;
int n = moves.size();
cout << "Balance Moves: " << endl;
for(int i = 0;i<n;i++){
    cout << "(" << moves[i][0][0] << ", " << moves[i][0][1] << ") -> (" << moves[i][1][0] << ", " << moves[i][1][1] << ")" << endl;
}
cout << endl;

if(Algorithm == 1 || Algorithm == 2){
    cout << "Rows: " << endl;
    n = rows.size();
    for(int i = 0; i<n; i++){
        cout << "Compress Row " << rows[i] << ": ";
        for(int j = 0;j<rowmoves[i].size();j++){
            cout << "(" << rowmoves[i][j][0] << " -> " << rowmoves[i][j][1] << ")";
            if(j!=rowmoves[i].size() - 1){
                cout << ", ";
            }
        }
        cout << endl;
    }
}

if(Algorithm == 1){
cout << endl;
cout << "Bank Moves: " << endl;
n = bankmoves.size();
for(int i = 0;i<n;i++){
    cout << "(" << bankmoves[i][0][0] << ", " << bankmoves[i][0][1] << ") -> (" << bankmoves[i][1][0] << ", " << bankmoves[i][1][1] << ")" << endl;
}
}
}
