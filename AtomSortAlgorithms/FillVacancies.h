#ifndef FILLVACANCIES_H
#define FILLVACANCIES_H

#include <vector>
#include <tuple>
#include <iostream>

using namespace std;

class FillVacancies
{
    public:
        vector<vector<int>> FindVacancies(vector<vector<bool>> Array, vector<int> Range);
        tuple<vector<vector<bool>>,vector<vector<vector<int>>>,vector<int>,vector<vector<vector<int>>>,vector<vector<int>>> Fill(vector<vector<bool>> Array, vector<vector<int>> bank, vector<vector<int>> vacancies,vector<int> Range);

    private:
        vector<vector<bool>> Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2);
        tuple<vector<bool>,vector<vector<int>>> CompressRow(vector<bool> row, vector<int> Range);
};

#endif // FILLVACANCIES_H
