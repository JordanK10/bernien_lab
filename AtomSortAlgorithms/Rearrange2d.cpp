#include "rearrange2d.h"

vector<vector<bool>> Hungarian::MakeBoolArray(int Dim, float Prob){
    int i = 0;
    int j = 0;
    vector<vector<bool>> matrix;
    vector<bool> row;
    while (i < Dim){
        while(j < Dim){
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            if (r <= Prob){
                row.push_back(true);
            }else{
                row.push_back(false);
            }
            j++;
        }
        i++;
        j = 0;
        matrix.push_back(row);
        row = {};

    }
    return matrix;
}
