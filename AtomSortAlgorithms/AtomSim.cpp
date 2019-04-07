#include "AtomSim.cpp"

vector<vector<vector<int>>> BalanceCompress::Interpolate(vector<int> movefrom, vector<int> moveto, int center){
    vector<vector<vector<int>>> moves;
    if(moveto[0] > movefrom[0]){
        for(int i = 0;i<(moveto[0] - center - 1);i++){
            moves.push_back({{moveto[0] -i - 1,moveto[1]},{moveto[0] - i,moveto[1]}});
        }
        moves.push_back({movefrom,{center + 1,moveto[1]}});
    }else{
        for(int i = 0;i<(center - moveto[0]);i++){
            moves.push_back({{moveto[0]+i+1,moveto[1]},{moveto[0]+i,moveto[1]}});
        }
        moves.push_back({movefrom,{center,moveto[1]}});
    }
    return moves;
}
