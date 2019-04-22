#include "choose.h"

choose::choose(vector<int> start, int n)
{
    setOfVals = start;
    go(0,n);
}


void choose::go(int offset, int k) {
  if (k == 0) {
    r.push_back(combination);
    return;
  }
  for (int i = offset; i <= setOfVals.size() - k; ++i) {
    combination.push_back(setOfVals[i]);
    go(i+1, k-1);
    combination.pop_back();
  }
}
