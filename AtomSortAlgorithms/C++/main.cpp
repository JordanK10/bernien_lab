#include <iostream>
#include <random>
#include <vector>
#include <ctime>
#include <fstream>
#include <chrono>
#include "choose.h"
#include <tuple>

using namespace std;

vector<vector<bool>> MakeBoolArray(int Dim, float Prob){
    int i = 0;
    int j = 0;
    srand(time(NULL)*time(NULL));
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

tuple<bool,vector<int>,vector<int>> test(vector<vector<bool>> Array,int n)
{
    int total = 0;
    int total2 = 0;
    vector<int> rowTotals;
    vector<int> colTotals;
    vector<int> rows;
    vector<int> cols;
    for(int i = 0; i<Array.size();i++){
        for(int j = 0;j<Array.size();j++){
            total += Array[i][j];
            total2 += Array[j][i];
        }
        rowTotals.push_back(total);
        colTotals.push_back(total2);
        total = 0;
        total2 = 0;
    }
    for(int i = 0;i<Array.size();i++){
        if(rowTotals[i]>=n){
            rows.push_back(i);
        }
    }
    for(int i = 0;i<Array.size();i++){
        if(colTotals[i]>=n){
            cols.push_back(i);
        }
    }
    bool goodArray = true;
    if(cols.size()<n || rows.size()<n){
        goodArray = false;
    }
    return make_tuple(goodArray,rows,cols);
}

bool check(vector<vector<bool>> Array,vector<int> rows, vector<int> cols)
{
    bool good = true;
    for(int i = 0;i<rows.size();i++){
        for(int j = 0;j<cols.size();j++){
            if(Array[rows[i]][cols[j]] == false){
                good = false;
                break;
            }
        }
        if(good == false){
            break;
        }
    }
    return good;
}

void printArray(vector<vector<bool>> Array)
{
    for(int i = 0;i<Array.size();i++){
        for(int j = 0;j<Array.size();j++){
            cout << Array[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

bool in(vector<int> row, int n)
{
    bool yes = false;
    for(int i = 0;i<row.size();i++){
        if(row[i] == n){
            yes = true;
            break;
        }
    }
    return yes;
}

vector<vector<bool>> arrayAdjust(vector<vector<bool>> Array,vector<int> rows,vector<int> cols)
{
    vector<vector<bool>> newArray = Array;
    for(int i = 0;i<Array.size();i++){
        for(int j = 0;j<Array.size();j++){
            if(!in(rows,i)||!in(cols,j)){
                newArray[i][j] = false;
            }
        }
    }
    return newArray;
}

int main() {

    float loadProb = .7;
    int counter2 = 0;
    int N = 10;
    vector<vector<bool>> Array;
    vector<int> rows;
    vector<int> cols;
    vector<vector<int>> rowCombs;
    vector<vector<int>> colCombs;
    bool good;
            int bankSize;
ofstream myFile("AtomDropBankSize.nb");
myFile << "data = {";
for(float loadProb = .6;loadProb<.999;loadProb+=.01){
    cout << loadProb << endl;
    for(int n = 4;n<10;n++){
        bankSize = 0;
        for(int k =0;k<1000;k++){
            Array = MakeBoolArray(N,loadProb);
            tie(good,rows,cols) = test(Array,n);
            vector<int> newRows;
            vector<int> newCols;
            vector<int> bankRows;
            vector<int> bankCols;
            bool success = false;
            if(good == true){
                choose rowObj(rows,n);
                choose colObj(cols,n);
                rowCombs = rowObj.r;
                colCombs = colObj.r;
                for(int i = 0;i<rowCombs.size();i++){
                    for(int j = 0;j<colCombs.size();j++){
                        success = check(Array,rowCombs[i],colCombs[j]);
                        if(success == true){
                            newRows = rowCombs[i];
                            newCols = colCombs[j];
                            break;
                        }
                    }
                    if(success == true){
                        break;
                    }
                }
            }

            if(success == true && good == true){
                int r1;
                int r2;

                for(int i = 0;i<newRows[0];i++){
                    bankRows.push_back(i);
                }
                r1 = newRows[0];
                r2 = Array.size() - 1;


                if(newRows[newRows.size() - 1]<Array.size()-1){
                r2 = newRows[newRows.size() - 1];
                for(int i = newRows[newRows.size() - 1] + 1;i<Array.size();i++){
                    bankRows.push_back(i);
                }}
                for(int i = 0;i<newCols[0];i++){
                    bankCols.push_back(i);
                }

                if(newCols[newCols.size() - 1]<Array.size()-1){
                for(int i = newCols[newCols.size() - 1] + 1;i<Array.size();i++){
                    bankCols.push_back(i);
                }}
                for(int i = 0;i<bankRows.size();i++){
                    for(int j = 0; j<Array.size();j++){
                        bankSize += Array[bankRows[i]][j];
                    }
                }
                for(int j = 0;j<bankCols.size();j++){
                    for(int i = r1;i<=r2;i++){
                        bankSize += Array[i][bankCols[j]];
                    }
                }


            }else{
                bankSize = 0;
            }

            }
            counter2++;
            myFile << "{" << loadProb << "," << n << "," << bankSize/1000 <<"}";
            if(n == 9 &&loadProb>.989){
                myFile << "};\nListDensityPlot[data,PlotLegends -> Automatic,PlotRange->Full]";
            }else{
                myFile << ",";
            }
            if(counter2%7 == 0){myFile<<"\n";}
    }
}
myFile.close();

    return 0;
}

