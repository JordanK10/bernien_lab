#include "FillVacancies.h"

using namespace std;
vector<vector<int>> FillVacancies::FindVacancies(vector<vector<bool>> Array, vector<int> Range){
    int Height = Array.size();
    vector<vector<int>> vacant = {};
    for(int i = 0; i<Height;i++){
        for(int j = Range[0];j<=Range[1];j++){
            if(Array[i][j] == false){
                vacant.push_back({i,j});
            }
        }
    }
    return vacant;
}


vector<vector<bool>> FillVacancies::Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2){
    srand(time(NULL));
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    vector<vector<bool>> NewArray = Array;
    if(r>moveFidelity){
        NewArray[pos1[0]][pos1[1]] = false;
        NewArray[pos2[0]][pos2[1]] = false;
    }else{
        NewArray[pos1[0]][pos1[1]] = false;
        NewArray[pos2[0]][pos2[1]] = true;
    }
    return NewArray;
}

tuple<vector<bool>,vector<vector<int>>> FillVacancies::CompressRow(vector<bool> row, vector<int> Range){
    int dim = row.size();
    int r1 = Range[0];
    int r2 = Range[1];
    int suff  = r2 - r1 + 1;
    int atoms = 0;
    vector<int> pos = {};
    for(int i = 0;i<dim;i++){
        if( row[i] == true){
            atoms ++;
            pos.push_back(i);
        }
    }
    int diff = atoms - suff;
    int missing = 0;
    if(diff<0){
        missing = -diff;
    }
    vector<vector<int>> moves = {};
    int j = 0;
    int k = 0;
    vector<bool> newRow = {};
    int extras = diff;


    while(j<diff && j<r1){
        newRow.push_back(true);
        moves.push_back({pos[k],j});
        j ++;
        k ++;
        extras--;
    }
    while(j<r1 + missing){
        newRow.push_back(false);
        j++;
    }
    while(j<=r2){
        newRow.push_back(true);
        moves.push_back({pos[k],j});
        k++;
        j++;
    }
    while(j<dim-extras){
        newRow.push_back(false);
        j++;
    }
    while(j<dim){
        newRow.push_back(true);
        moves.push_back({pos[k],j});
        k ++;
        j++;
    }
    return make_tuple(newRow,moves);
}

tuple<vector<vector<bool>>,vector<vector<vector<int>>>,vector<int>,vector<vector<vector<int>>>,vector<vector<int>> > FillVacancies::Fill(vector<vector<bool>> Array, vector<vector<int>> bank, vector<vector<int>> vacancies,vector<int> Range)
{
    vector<vector<vector<int>>> moves;
    vector<int> rows;
    vector<vector<vector<int>>> rowmoves = {};

    int Height = Array.size();
    int Width = Array[0].size();

    for(int i = 0;i<Height;i++){
        rowmoves.push_back({{0}});
    }

    int v = vacancies.size();
    for(int i = 0;i<v;i++){
        bool done = false;
        int row = vacancies[i][0];
        for(int j = 0; j<bank.size();j++){
            if(bank[j][0] == row){
                tie(Array[row],rowmoves[row]) = CompressRow(Array[row],Range);
                rows.push_back(row);
                bank.erase(bank.begin()+j);
                done = true;
                break;
            }
        }
        if(done == false){
            for(int k = row;k<Height;k++){
                for(int l = 0;l<bank.size();l++){
                    if(bank[l][0] == k){
                        Array = Move(Array,bank[l],{row,bank[l][1]});
                        moves.push_back({bank[l],{row,bank[l][1]}});
                        bank.erase(bank.begin() + l);
                        done = true;
                        break;
                    }
                }
                if(done == true){
                    break;
                }
            }
            if(done == false){
                for(int k = row;k>= 0;k--){
                    for(int l = 0;l<bank.size();l++){
                        if(bank[l][0] == k){
                            Array = Move(Array,bank[l],{row,bank[l][1]});
                            moves.push_back({bank[l],{row,bank[l][1]}});
                            bank.erase(bank.begin() + l);
                            done = true;
                            break;
                        }
                    }
                if(done == true){
                    break;
                    }
                }
            }

            tie(Array[row],rowmoves[row]) = CompressRow(Array[row],Range);
            rows.push_back(row);
        }

    }
    return make_tuple(Array,moves,rows,rowmoves,bank);
}
