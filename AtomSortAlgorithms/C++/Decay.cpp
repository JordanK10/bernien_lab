#include "Decay.h"

using namespace std;

vector<vector<double>> Decay::MakeLossTime(int n, int m, double lifetime){
vector<vector<double>> LossTime = {};
vector<double> row = {};
for(int i = 0;i<n;i++){
    for(int j = 0;j<m;j++){
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        double losstime = -lifetime*log(1 - r);
        row.push_back(losstime);
    }
    LossTime.push_back(row);
    row = {};
}
return LossTime;
}

bool Decay::AtomLoss(double duration,double lifetime)
{
    if(lifetime<duration){
        return false;
    }else{
        return true;
    }
}

vector<vector<bool>> Decay::ArrayDecay(vector<vector<bool>> Array, double time,vector<vector<double>> LossTime){
    int n = Array.size();
    int m = Array[0].size();
    for(int i = 0;i<n;i++){
        for(int j = 0;j<m;j++){
            if(Array[i][j] == true){
                Array[i][j] = AtomLoss(time, LossTime[i][j]);
            }
        }
    }
    return Array;
}
