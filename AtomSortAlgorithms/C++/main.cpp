#include <iostream>
#include <random>
#include <vector>
#include "Timer.h"
#include <stdlib.h>
#include "RectBC.h"
#include "FillVacancies.h"
#include "Windows.h"
#include "Decay.h"
#include <fstream>
#include <math.h>

using namespace std;
int main(){

double halflife = 100; //seconds
double Camera_freq = 1; //Hz
int movetime = 1; //milliseconds


vector<vector<bool>> Array;
vector<vector<vector<int>>> moves;
vector<vector<int>> bank;
vector<int> ColRange;
vector<int> compressedrows;
vector<int> rows;
vector<vector<vector<int>>> rowmoves;
vector<vector<int>> vacant;

RectBC obj(100,.6);
Decay obj3;
FillVacancies obj2;

halflife *= 1000000000;
double sleeptime = 1000/Camera_freq;

vector<vector<double>> LossTime = obj3.MakeLossTime(14,14,halflife);
Timer time;
tie(Array,moves,bank,compressedrows,ColRange) = obj.BalanceCompress(obj.Array);

int BankSize =  bank.size();

vector<vector<bool>> StaticArray = Array;
vacant = obj2.FindVacancies(Array,ColRange);
Sleep((compressedrows.size() + moves.size())*movetime);
double downtime = time.gettime();

while(1){

    Sleep(sleeptime);

    time.pause();
    Array = obj3.ArrayDecay(Array,time.gettime(),LossTime);
    time.restart();

    vacant = obj2.FindVacancies(Array,ColRange);

    if(vacant.size() <= bank.size()){
        tie(Array, moves, rows, rowmoves, bank) = obj2.Fill(Array,bank,vacant,ColRange);
        for(int i = 0;i<vacant.size();i ++){
            LossTime[vacant[i][0]][vacant[i][1]] = halflife;
            }
        Sleep((moves.size() + rows.size())*movetime);
        downtime += (moves.size() + rows.size())*movetime;
    }else{
        time.pause();
        break;
    }
}
cout << "Bank Size: " << BankSize << endl;
cout << "Expected Time: " << halflife/49/1000000000*(BankSize+1) << " s" << endl;
cout << "Actual Time: " << time.gettime()/1000000000 << " s" << endl;
cout << "Array Lifetime: " << halflife/49/1000000000 <<" s" << endl;
cout << "Downtime: " << downtime/1000000 << " ms" << endl;
}
