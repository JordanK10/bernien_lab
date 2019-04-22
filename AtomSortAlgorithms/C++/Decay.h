#ifndef DECAY_H
#define DECAY_H

#include <iostream>
#include <random>
#include <vector>
#include <math.h>
#include <ctime>

using namespace std;

class Decay
{
    public:
        Decay();
        vector<vector<double>> MakeLossTime(int n, int m, double lifetime);
        bool AtomLoss(double duration,double lifetime);
        vector<vector<bool>> ArrayDecay(vector<vector<bool>> Array, double time,vector<vector<double>> LossTime);
        vector<double> MakeBankLossTime(int n, double lifetime);
        vector<vector<int>> BankDecay(vector<vector<int>> Bank, double time,vector<double> BankLossTime);

    private:
};

#endif // DECAY_H
