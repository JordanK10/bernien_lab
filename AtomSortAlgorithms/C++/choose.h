#ifndef CHOOSE_H
#define CHOOSE_H

#include <vector>

using namespace std;

class choose
{
    public:
        choose(vector<int> start, int n);
        vector<vector<int>> r;
        vector<int> combination;
        void go(int offset, int k);
        vector<int> setOfVals;

    private:

};

#endif // CHOOSE_H
