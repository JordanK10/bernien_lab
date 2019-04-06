#ifndef DECAY_H
#define DECAY_H

#include <iostream>
#include <random>
#include <vector>
#include <math.h>

using namespace std;

class Decay
{
    public:
        vector<vector<double>> MakeLossTime(int n, int m, double lifetime);
        bool AtomLoss(double duration,double lifetime);
        vector<vector<bool>> ArrayDecay(vector<vector<bool>> Array, double time,vector<vector<double>> LossTime);

    private:
};

#endif // DECAY_H
