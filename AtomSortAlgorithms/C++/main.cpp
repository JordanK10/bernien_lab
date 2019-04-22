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
#include <fstream>

using namespace std;
int main(){

double halflife = 10; //seconds
double Camera_freq = 10; //Hz
int movetime = 1; //milliseconds
double moveFidelity = .997;

double speedup = 100;

vector<vector<bool>> Array;
vector<vector<vector<int>>> moves;
vector<vector<int>> bank;
vector<int> ColRange;
vector<int> compressedrows;
vector<int> rows;
vector<vector<vector<int>>> rowmoves;
vector<vector<int>> vacant;

RectBC obj;
Decay obj3;
FillVacancies obj2;

Camera_freq*= speedup;
obj2.moveFidelity = moveFidelity;
obj.moveFidelity = moveFidelity;
halflife *= 1E9;
double sleeptime = 1000/Camera_freq;

ofstream myFile("10sBankDecay.csv");
myFile << "Bank Size,Time, Downtime\n";
Timer time;
time.speedup = speedup;

for(int j = 0;j<1000;j++){
cout << j<< endl;
Array = obj.MakeRectArray(100,.6);

bool bad = false;
tie(Array,moves,bank,compressedrows,ColRange,bad) = obj.BalanceCompress(Array);
vector<vector<double>> LossTime = obj3.MakeLossTime(Array.size(),Array[0].size(),halflife);
vector<double> BankLossTime = obj3.MakeBankLossTime(bank.size(),halflife);
int BankSize =  bank.size();

double downtime = time.gettime();
while(!bad){
    Sleep(sleeptime);

    time.pause();
    Array = obj3.ArrayDecay(Array,time.gettime(),LossTime);
    bank = obj3.BankDecay(bank,time.gettime(),BankLossTime);

    time.restart();

    vacant = obj2.FindVacancies(Array,ColRange);

    if(vacant.size() <= bank.size() && vacant.size() != 0){
        tie(Array, moves, rows, rowmoves, bank) = obj2.Fill(Array,bank,vacant,ColRange);
        for(int i = 0;i<vacant.size();i ++){
            LossTime[vacant[i][0]][vacant[i][1]] = halflife;
            }
        Sleep((moves.size() + rows.size())*movetime);
        downtime += (moves.size() + rows.size())*movetime;
    }
    if(vacant.size() > bank.size()){
        time.pause();
        break;
    }
}
if(bad == false){
myFile << BankSize << "," << time.gettime()/1E9 << "," << downtime/1E9 << "\n";
}
time.zero();
}
/*
cout << "Bank Size: " << BankSize << endl;
cout << "Actual Time: " << time.gettime()/1000000000 << " s" << endl;
cout << "Downtime: " << downtime/1000000 << " ms" << endl;
*/
myFile.close();
}
