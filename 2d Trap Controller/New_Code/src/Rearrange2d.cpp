#include "Rearrange2d.h"
#include <vector>
#include <tuple>
#include <iostream>
#include <random>
#include <math.h>

using namespace std;

int i;
int j;
int k;
int COM[2];
int g_counter;

void printArray(vector<vector<bool>> Array){
  int d1 = Array.size();
  int d2 = Array[0].size();
  cout << endl;
  for(int s1 = 0;s1<d1;s1++){
    for(int s2 = 0;s2<d2;s2++){
        cout << Array[s1][s2] << " ";

    }
    cout << endl;
  }
};


//////////////////BALANCE_COMPRESS////////////////////////////


vector<bool> ColumnAt(vector<vector<bool>> Array, int dim){
  vector<bool> new_col(Array.size());
  for(k=0;k<Array.size();k++)
    new_col[k]=Array[k][dim];
  return new_col;
}

vector<bool> CompressRow(vector<bool> row, int left, int right, int atoms){
    int suff = right - left + 1;
    int diff = atoms - suff;
    int extras = diff;
    int dim = row.size();
    j = 0;
    vector<bool> newRow(row.size());

    while(j<diff && j<left){
        newRow[j] = true;
        j ++;
        atoms--;
        extras--;
    }
    while(j<left){
        newRow[j] = false;
        j++;
    }
    while(j<=right && atoms>0){
        newRow[j] = true;
        atoms --;
        j++;
    }
    while(j<dim-extras){
        newRow[j] = false;
        j++;
    }
    while(j<dim){
        newRow[j] = false;
        j++;
    }
    return newRow;
}

vector<int> RowSum(vector<vector<bool>> Array){
    vector<int> RowTotal(Array.size()+1);
    for(i=0; i < Array.size(); i++){
        for(j=0; j < Array[0].size(); j++){
            RowTotal[i+1] += int(Array[i][j]);
            RowTotal[0] += int(Array[i][j]);
        }
    }
    return RowTotal;
}

vector<int> ColSum(vector<vector<bool>> Array){
  vector<int> ColTotals(Array[0].size() + 1);
  for(i = 0;i<Array[0].size();i++){
    for(j = 0;j<Array.size();j++){
      ColTotals[i+1] += int(Array[j][i]);
      ColTotals[0] += int(Array[j][i]);
    }
  }
  return ColTotals;
}

vector<vector<int>> Balance(vector<vector<bool>> &Array, vector<int> &Range, int SufficientAtoms, vector<int> &RowTotal,vector<RearrangementMove> &moves){
    int dim = Array[0].size();
    int center;
    if((Range[1] - Range[0])%2 == 0){
        center  = (Range[1] - Range[0])/2 + Range[0];
    }else{
        center  = (Range[1] - Range[0] - 1)/2 + Range[0];
    }
    int SuffLower = (center - Range[0] + 1)*SufficientAtoms;
    int SuffUpper = (Range[1] - center)*SufficientAtoms;


    i = Range[0];
    int Lower = 0;
    int Upper = 0;
    while(i <= center){
        Lower += RowTotal[i+1];
        i ++;
    }
    while(i <= Range[1]){
        Upper += RowTotal[i+1];
        i ++;
    }

    if(SuffUpper + SuffLower > Upper + Lower){
      cout << "tenemos un problema" << endl;
    }
    while(SuffUpper > Upper || SuffLower > Lower){
        i = center + 1;
        vector<signed int> moveto = {-1,-1};
        vector<signed int> movefrom = {-1,-1};
        j = 0;
        if(SuffLower < Lower && SuffUpper < Upper){
            break;
            }
        if(SuffLower < Lower && SuffUpper > Upper){
            while(j < dim){
                while(i <= Range[1]){
                    if(Array[i][j] == false){
                        moveto = {i,j};
                        k = center;
                        while(k >= Range[0]){
                            if(Array[k][j] == true){
                                movefrom = {k,j};
                                break;
                            }
                            k --;
                        }
                    }
                    if(moveto[0] == -1 || movefrom[0] == -1){
                        i ++;
                    }else{
                        break;
                    }
                }
                if(moveto[0] == -1 || movefrom[0] == -1){
                        i = center + 1;
                        j ++;
                        moveto = {-1,-1};
                        movefrom = {-1,-1};
                    }else{
                        Upper ++;
                        break;
                }
            }
        }
        i = center;
        j = 0;
        if(SuffLower > Lower && SuffUpper < Upper){
            while(j < dim){
                while(i >= Range[0]){
                    if(Array[i][j] == false){
                        moveto = {i,j};
                        k = center + 1;
                        while(k <= Range[1]){
                            if(Array[k][j] == true){
                                movefrom = {k,j};
                                break;
                            }
                            k ++;
                        }
                    }
                    if(moveto[0] == -1 || movefrom[0] == -1){
                        i --;
                    }else{
                        break;
                    }
                }
                if(moveto[0] == -1 || movefrom[0] == -1){
                        i = center;
                        j ++;
                        moveto = {-1,-1};
                        movefrom = {-1,-1};
                    }else{
                        Lower ++;
                        break;
                }
        }
    }
    if(moveto[0] != -1 || movefrom[0] != -1){
    moves.push_back(RearrangementMove());
    moves[g_counter].dim = movefrom[1];
    moves[g_counter].row = false;
    moves[g_counter].startingConfig = ColumnAt(Array,movefrom[1]);
    Array[movefrom[0]][movefrom[1]] = false;
    Array[moveto[0]][moveto[1]] = true;
    moves[g_counter].endingConfig = ColumnAt(Array,movefrom[1]);
    g_counter ++;

    RowTotal[movefrom[0]+1] --;
    RowTotal[moveto[0]+1] ++;
    }
}
return {{Range[0],center},{center + 1,Range[1]}};
}

vector<vector<bool>> assignCol(vector<vector<bool>> Array, vector<bool> col, int index)
{
    for(i = 0;i<Array.size();i++){
        Array[i][index] = col[i];
    }
    return Array;
}

int* CenterOfMass(vector<vector<bool>> Array){
    int dim1 = Array.size();
    int dim2 = Array[0].size();
    float RowWeight = 0;
    float ColWeight = 0;
    float TotalWeight = 0;
    for(i=0; i < dim1; i++){
        for(j=0; j < dim2; j++){
            if (Array[i][j] == true){
                RowWeight += i;
                ColWeight += j;
                TotalWeight ++;
            }
        }
    }

    COM[0] = int(RowWeight /= TotalWeight);
    COM[1] = int(ColWeight /= TotalWeight);
    int *ptr = COM;
    return ptr;
}


vector<RearrangementMove> BalanceCompressAlg(vector<vector<bool>> Array, int mode){

    int ArrayDim = Array.size();
    vector<int> RowTotals = RowSum(Array);
    int atoms = RowTotals[0];
    int TargetDim = sqrt(atoms);

    vector<RearrangementMove> moves;


    int col_y;
    int col_z;
    int row_y;
    int row_z;

    //default mode set center of mass as center of array
    if(mode == CENTER_COM){
        int *COM = CenterOfMass(Array);

        float x = TargetDim/2;

            col_y = COM[1] - x;
            col_z = TargetDim + col_y - 1;

            if(col_y < 0){
                col_y = 0;
                col_z = TargetDim - 1;
            }
            if(col_z >= ArrayDim){
                col_z = ArrayDim - 1;
                col_y = ArrayDim - TargetDim;
            }

            row_y = COM[0] - x;
            row_z = TargetDim + row_y - 1;

            if(row_y < 0){
                row_y = 0;
                row_z = TargetDim - 1;
            }
            if(row_z >= ArrayDim){
                row_z = ArrayDim - 1;
                row_y = ArrayDim - TargetDim;
            }
    }

    //mode 5 = corner nearest the COM
    if(mode == CLOSE_CORNER){
        int *COM = CenterOfMass(Array);
        if(COM[0] < (ArrayDim - 1)/2){
            if(COM[1]<(ArrayDim - 1)/2){
                mode = UL_CORNER;
            }else{
                mode = UR_CORNER;
            }
        }else{
            if(COM[1]<(ArrayDim - 1)/2){
                mode = LL_CORNER;
            }else{
                mode = LR_CORNER;
            }
        }
    }

    //mode 1 = upper left corner
    if(mode == UL_CORNER){
        col_y = 0;
        col_z = TargetDim - 1;
        row_y = 0;
        row_z = TargetDim - 1;
    }

    //mode 2 = upper right corner
    if(mode == UR_CORNER){
        col_y = ArrayDim - TargetDim;
        col_z = ArrayDim - 1;
        row_y = 0;
        row_z = TargetDim - 1;
    }

    //mode 3 = lower left corner
    if(mode == LL_CORNER){
        col_y = 0;
        col_z = TargetDim - 1;
        row_y = ArrayDim - TargetDim;
        row_z = ArrayDim - 1;
    }

    //mode 4 = lower right corner
    if(mode == LR_CORNER){
        col_y = ArrayDim - TargetDim;
        col_z = ArrayDim - 1;
        row_y = ArrayDim - TargetDim;
        row_z = ArrayDim - 1;
    }

    if(mode == REC_LEFT || mode == REC_RIGHT || mode == REC_CENT){
        TargetDim = Array.size();
        row_y = 0;
        row_z = TargetDim - 1;
    }

    //mode 6 = rectangular, push left
    if(mode == REC_LEFT){
        col_y = 0;
        col_z = TargetDim - 1;
    }

    // mode 7 = rectangular, push right
    if(mode == REC_LEFT){
        col_y = Array[0].size() - TargetDim;
        col_z = Array[0].size() - 1;
    }

    //mode 8 = rectangular, center
    if(mode == REC_CENT){
        int *COM = CenterOfMass(Array);
        col_y = COM[1] - TargetDim/2;
        col_z = col_y + TargetDim - 1;
    }


        vector<bool> tempCol;
        vector<bool> compressedCol;
        bool check = false;

        vector<int> ColTotals = ColSum(Array);


if(mode == CENTER_COM||mode == UL_CORNER||mode == UR_CORNER||mode == LL_CORNER||mode == LR_CORNER||mode == CLOSE_CORNER){
      for(int kevin=0; kevin<ArrayDim; kevin++){
              atoms = 0;
              for(j = row_y; j<=row_z; j++){
                  atoms += RowTotals[j+1];
              }
              if(atoms>=TargetDim*TargetDim){
                  check = true;
                  break;
              }
              tempCol = ColumnAt(Array,kevin); // Extract desired column
              compressedCol = CompressRow(tempCol,row_y,row_z,ColTotals[kevin+1]);
              moves.push_back(RearrangementMove());
              moves[g_counter].row = false;
              moves[g_counter].startingConfig = tempCol;
              moves[g_counter].endingConfig = compressedCol;
              moves[g_counter].dim = kevin;
              Array = assignCol(Array,compressedCol,kevin);
              g_counter ++;
               // Push back column and compressed column
              RowTotals = RowSum(Array); // Recalculate row totals
          }
      if(check == false){
          col_z --;
          row_z --;
          TargetDim --;
      }
 }

    int s = 0;
    vector<vector<int>> Range1(2);
    int balancedRows = 0;
    vector<vector<int>> RowRange = {{row_y,row_z}};


    //balance rows until #of balanced rows = target dim
    while(balancedRows < TargetDim){
        Range1 = Balance(Array,RowRange[s],TargetDim,RowTotals,moves);
        if(Range1[0][1] == Range1[0][0]){
            balancedRows ++;
        }else{
            RowRange.push_back(Range1[0]);
        }
        if(Range1[1][1] == Range1[1][0]){
            balancedRows ++;
        }else{
            RowRange.push_back(Range1[1]);
        }
        s ++;
    }


    i = RowRange[0][0];
    while(i <= RowRange[0][1]){
        moves.push_back(RearrangementMove());
        moves[g_counter].dim = i;
        moves[g_counter].row = true;
        moves[g_counter].startingConfig = Array[i];
        moves[g_counter].endingConfig = CompressRow(Array[i],col_y,col_z,RowTotals[i]);

        Array[i] = CompressRow(Array[i],col_y,col_z,RowTotals[i]);

        i++;
        g_counter++;
    }


return moves;
}

///////////////////////////HUNGARIAN///////////////////////////////////////



void clear_covers(vector<bool> &a, vector<bool>&b){
    int n = a.size();
    for(int i = 0; i < n; i ++){
        a[i] = false;
        b[i] = false;
    }
}

void find_zero(vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered,signed int &row,signed int &col){
    row = -1;
    col = -1;
    int n = CostMatrix.size();
    int i = 0;
    bool done = false;
    int j;

    while(done == false){
        j = 0;
        while(1){
            if(CostMatrix[i][j] == 0 && row_covered[i] == false && col_covered[j] == false){
                row = i;
                col = j;
                done = true;
            }
            j += 1;
            if(j>=n){
                break;
            }
        }
        i += 1;
        if(i>= n){
            done = true;
        }
    }
}

int find_star_row(vector<vector<int>> marked, int row){
    signed int col = -1;
    int n = marked[0].size();
    for(int j = 0; j<n;j++){
        if(marked[row][j] == 1){
            col = j;
            break;
        }
    }
    return col;
}

int find_star_col(vector<vector<int>> marked, int col){
    signed int row = -1;
    int n = marked.size();
    for(int j = 0; j<n;j++){
        if(marked[j][col] == 1){
            row = j;
            break;
        }
    }
    return row;
}

int find_prime_row(int row, vector<vector<int>> marked){
        signed int col = -1;
        int n = marked.size();
        for(int j = 0;j<n;j++){
            if(marked[row][j] == 2){
                col = j;
                break;
            }
        }
        return col;
}

void erase_primes(vector<vector<int>> &marked){
    int n = marked.size();
    for(int i = 0; i<n; i ++){
        for(int j = 0; j<n;j++){
            if(marked[i][j] == 2){
                marked[i][j] = 0;
            }
        }
    }
}

void convert_path(int counter, vector<vector<int>> path, vector<vector<int>> &marked){
    for(int i = 0; i<counter + 1; i++){
        if(marked[path[i][0]][path[i][1]] == 1){
            marked[path[i][0]][path[i][1]] = 0;
        }else{
            marked[path[i][0]][path[i][1]] = 1;
        }
    }
}

int find_smallest(vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered){
    int n = CostMatrix.size();
    int minval = n*n;
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            if(row_covered[i] == false && col_covered[j] == false){
                if(minval > CostMatrix[i][j]){
                    minval = CostMatrix[i][j];
                }
            }
        }
    }
    return minval;
}
int metric(vector<int> pos1, vector<int> pos2,int dim){
    signed int x = pos1[0] - pos2[0];
    signed int y = pos1[1] - pos2[1];
    int distance = 0;
    if(x<0){
        x = -x;
    }
    if(y<0){
        y = -y;
    }
    if(x!=0 && y!=0){
        distance += 2*dim*dim;
    }
    distance += x*x + y*y;
    return distance;
}

tuple<vector<vector<int>>,vector<vector<int>>, int> ToCostMatrix(vector<vector<bool>> InitArray, vector<vector<int>> TargetArray, int Dim1){
    vector<vector<int>> InitPositions = {};
    int NumberOfPositions = TargetArray.size();
    int i = 0;
    int j = 0;
    int atoms = 0;
    while(i<Dim1){
        while(j<Dim1){
            if(InitArray[i][j] == true){
                InitPositions.push_back({i,j});
                atoms += 1;
            }
            j += 1;
        }
        i += 1;
        j = 0;
    }
    i = 0;
    j = 0;
    vector<int> row = {};
    vector<vector<int>> CostMatrix = {};
    int difference = atoms - NumberOfPositions;
    if(difference>=0){
        while(i<NumberOfPositions){
            while(j<atoms){
                int dist = metric(InitPositions[j],TargetArray[i],Dim1);
                row.push_back(dist);
                j += 1;
            }
            CostMatrix.push_back(row);
            j = 0;
            i += 1;
            row = {};
        }
        i = 0;
        row = {};
        j = 0;
        while(j<atoms){
            row.push_back(0);
            j += 1;
        }
        while(i < difference){
            CostMatrix.push_back(row);
            i += 1;
        }
    }
    return make_tuple(CostMatrix, InitPositions, difference);
}

vector<vector<int>> ToTargetArray(int dim, int r0,int r1, int c0 ,int c1){
    vector<vector<int>> TargetArray = {};
    int i = r0;
    int j = c0;
    while(i <= r1){
        while(j <= c1){
            TargetArray.push_back({i,j});
            j+= 1;
        }
        i += 1;
        j = c0;
    }
    return TargetArray;
}

vector<vector<vector<int>>> interpret_results(vector<vector<int>> InitPositions, vector<vector<int>> TargetArray, vector<vector<int>> results){
    int n = results.size();
    int t = TargetArray.size();
    vector<vector<vector<int>>> moves = {};
    vector<int> moveto;
    vector<int> movefrom;
    for(int i = 0; i<n;i++){
        if(i<t && TargetArray[results[i][0]] != InitPositions[results[i][1]]){
            moveto = TargetArray[results[i][0]];
            movefrom = InitPositions[results[i][1]];
            moves.push_back({movefrom, moveto});
        }
    }
    return moves;
}

vector<vector<vector<int>>> order(vector<vector<vector<int>>> moves, vector<vector<bool>> Array){
        int i = 0;
        int n = moves.size();
        cout << n;
        vector<int> todo = {};
        vector<int> todo1 = {};
        int m;
        vector<vector<vector<int>>> properorder = {};
        while(i < n){
            if(Array[moves[i][1][0]][moves[i][1][1]] == false){
                Array[moves[i][0][0]][moves[i][0][1]] = false;
                Array[moves[i][1][0]][moves[i][1][1]] = true;
                properorder.push_back(moves[i]);
            }else{
                todo.push_back(i);
            }
            i ++;
        }
        int j;
        int k;
        while(1){
            todo1 = todo;
            todo = {};
            m = todo1.size();
            for(i = 0;i<m;i++){
                j = todo1[i];
                if(Array[moves[j][1][0]][moves[j][1][1]] == false){
                    Array[moves[j][0][0]][moves[j][0][1]] = false;
                    Array[moves[j][1][0]][moves[j][1][1]] = true;
                    properorder.push_back(moves[j]);
                }else{
                    todo.push_back(j);
                }
            }
            if(todo.size() == 0){
                break;
            }
        }
    return properorder;
}

int minfind(vector<int> a){
    int n = a.size();
    int minval = a[0];
    for(int i = 0; i < n;i++){
        if(a[i]<minval){
            minval = a[i];
        }
    }
    return minval;
}

int step1(vector<vector<int>> &CostMatrix){
    int minval;
    int n = CostMatrix.size();
    for(int i = 0;i<n;i++){
        minval = minfind(CostMatrix[i]);
        for(int j = 0; j < n;j++){
            CostMatrix[i][j] -= minval;
        }
    }
    return 2;
}

int step2(vector<vector<int>> &CostMatrix, vector<bool> &row_covered, vector<bool> &col_covered, vector<vector<int>> &marked){
    int n = CostMatrix.size();
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n;j++){
            if(CostMatrix[i][j] == 0 && col_covered[j] == false && row_covered[i] == false){
                marked[i][j] = 1;
                col_covered[j] = true;
                row_covered[i] = true;
                break;
            }
        }
    }
    clear_covers(row_covered,col_covered);
    return 3;
}

int step3(vector<vector<int>> &CostMatrix, vector<bool> &col_covered, vector<vector<int>> &marked){
    int step;
    int n = CostMatrix.size();
    int counter  = 0;
    for(int i = 0;i<n;i++){
        for(int j = 0;j<n;j++){
            if(marked[i][j] == 1 && col_covered[j] == false){
                col_covered[j] = true;
                counter += 1;
            }
        }
    }
    if(counter >= n){
        step = 0;
    }else{
        step = 4;
    }
    return step;
}

int step4(vector<vector<int>> &CostMatrix, vector<bool> &row_covered, vector<bool> &col_covered, vector<vector<int>> &marked, int &Z0_c, int &Z0_r){
    int step = 0;
    bool done = false;
    signed int row = -1;
    signed int col = -1;
    signed int star_col = -1;
    while(done == false){
        find_zero(CostMatrix,row_covered,col_covered,row,col);
        if(row<0){
            done = true;
            step = 6;
        }else{
            marked[row][col] = 2;
            star_col = find_star_row(marked, row);
            if(star_col >= 0){
                col = star_col;
                row_covered[row] = true;
                col_covered[col] = false;
            }else{
                done = true;
                Z0_r = row;
                Z0_c = col;
                step = 5;
            }
        }
    }
    return step;
}

int step5(vector<bool> &row_covered, vector<bool> &col_covered, vector<vector<int>> &marked, vector<vector<int>> &path, int &Z0_c, int &Z0_r){
    int counter = 0;
    path[counter][0] = Z0_r;
    path[counter][1] = Z0_c;
    signed int row;
    signed int col;
    bool done = false;
    while(done == false){
        row = find_star_col(marked,path[counter][1]);
        if(row>= 0){
            counter += 1;
            path[counter][0] = row;
            path[counter][1] = path[counter - 1][1];
        }else{
            done = true;
        }
        if(done == false){
            col = find_prime_row(path[counter][0], marked);
            counter += 1;
            path[counter][0] = path[counter-1][0];
            path[counter][1] = col;
        }
    }
    convert_path(counter, path, marked);
    clear_covers(row_covered,col_covered);
    erase_primes(marked);
    return 3;
}

int step6(vector<vector<int>> &CostMatrix, vector<bool> &row_covered, vector<bool> &col_covered){
    int minval = find_smallest(CostMatrix, row_covered, col_covered);
    int n = CostMatrix.size();
    for(int i = 0; i<n; i++){
        for(int j = 0; j<n; j++){
            if(row_covered[i] == true){
                CostMatrix[i][j] += minval;
            }
            if(col_covered[j] == false){
                CostMatrix[i][j] -= minval;
            }
        }
    }
    return 4;
}

vector<RearrangementMove> compute(vector<vector<bool>> Matrix,rearrange_mode mode){

    int ArrayDim = Matrix.size();
    int TargetDim;
    int atoms = 0;
    for(int i = 0;i<ArrayDim;i++){
        for(int j = 0;j<ArrayDim;j++){
            if(Matrix[i][j] == true){
                atoms ++;
            }
        }
    }
    TargetDim = sqrt(atoms);

    int col_y,col_z,row_y,row_z;

    if(mode == CENTER_COM){
        int *COM = CenterOfMass(Matrix);

        float x = TargetDim/2;

            col_y = COM[1] - x;
            col_z = TargetDim + col_y - 1;

            if(col_y < 0){
                col_y = 0;
                col_z = TargetDim - 1;
            }
            if(col_z >= ArrayDim){
                col_z = ArrayDim - 1;
                col_y = ArrayDim - TargetDim;
            }

            row_y = COM[0] - x;
            row_z = TargetDim + row_y - 1;

            if(row_y < 0){
                row_y = 0;
                row_z = TargetDim - 1;
            }
            if(row_z >= ArrayDim){
                row_z = ArrayDim - 1;
                row_y = ArrayDim - TargetDim;
            }
    }

    //mode 5 = corner nearest the COM
    if(mode == CLOSE_CORNER){
        int *COM = CenterOfMass(Matrix);
        if(COM[0] < (ArrayDim - 1)/2){
            if(COM[1]<(ArrayDim - 1)/2){
                mode = UL_CORNER;
            }else{
                mode = UR_CORNER;
            }
        }else{
            if(COM[1]<(ArrayDim - 1)/2){
                mode = LL_CORNER;
            }else{
                mode = LR_CORNER;
            }
        }
    }

    //mode 1 = upper left corner
    if(mode == UL_CORNER){
        col_y = 0;
        col_z = TargetDim - 1;
        row_y = 0;
        row_z = TargetDim - 1;
    }

    //mode 2 = upper right corner
    if(mode == UR_CORNER){
        col_y = ArrayDim - TargetDim;
        col_z = ArrayDim - 1;
        row_y = 0;
        row_z = TargetDim - 1;
    }

    //mode 3 = lower left corner
    if(mode == LL_CORNER){
        col_y = 0;
        col_z = TargetDim - 1;
        row_y = ArrayDim - TargetDim;
        row_z = ArrayDim - 1;
    }

    //mode 4 = lower right corner
    if(mode == LR_CORNER){
        col_y = ArrayDim - TargetDim;
        col_z = ArrayDim - 1;
        row_y = ArrayDim - TargetDim;
        row_z = ArrayDim - 1;
    }

    vector<vector<int>> TargetArray = ToTargetArray(TargetDim, row_y,row_z,col_y,col_z);
    vector<vector<int>> CostMatrix;
    vector<vector<int>> InitPositions;
    int diff;

    tie(CostMatrix,InitPositions, diff) = ToCostMatrix(Matrix,TargetArray,ArrayDim);
    int n = CostMatrix.size();
    vector<bool> row_covered(n);
    vector<bool> col_covered(n);
    int Z0_r = 0;
    int Z0_c = 0;
    vector<vector<int>> marked;
    vector<vector<int>> path;
    clear_covers(row_covered,col_covered);
    int i = 0;
    vector<int> row;
    for(i = 0; i < n; i++){
        row.push_back(0);
    }
    for(i = 0; i < n; i++){
        marked.push_back(row);
    }
    row = {};
    for(i = 0; i < n*2; i++){
        row.push_back(0);
    }
    for(i = 0; i < n*2; i++){
        path.push_back(row);
    }
    bool done = false;
    int step = 1;
    vector<vector<int>> results = {};
    while(step != 0){
        switch(step){
            case 1: step = step1(CostMatrix);
                break;
            case 2: step = step2(CostMatrix,row_covered,col_covered,marked);
                break;
            case 3: step = step3(CostMatrix,col_covered,marked);
                break;
            case 4: step = step4(CostMatrix,row_covered,col_covered,marked,Z0_c,Z0_r);
                break;
            case 5: step = step5(row_covered,col_covered,marked,path,Z0_c,Z0_r);
                break;
            case 6: step = step6(CostMatrix,row_covered,col_covered);
                break;
        }
    }

    for(i = 0; i < n; i++){
        for(int j = 0; j<n;j++){
            if(marked[i][j] == 1){
                results.push_back({i,j});
            }
        }
    }

    vector<vector<vector<int>>> moves = interpret_results(InitPositions, TargetArray, results);
    vector<vector<vector<int>>> ordered_moves = order(moves, Matrix);

    int moveNumber = ordered_moves.size();
    int counter  = 0;
    vector<RearrangementMove> rearrange_moves;
    for(i = 0;i<moveNumber;i++){
        if(ordered_moves[i][0][0] == ordered_moves[i][1][0]){
            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = true;
            rearrange_moves[counter].dim = ordered_moves[i][0][0];
            rearrange_moves[counter].startingConfig = Matrix[ordered_moves[i][0][0]];
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
            Matrix[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
            rearrange_moves[counter].endingConfig = Matrix[ordered_moves[i][0][0]];
            counter ++;
        }
        if(ordered_moves[i][0][1] == ordered_moves[i][1][1]){
            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = false;
            rearrange_moves[counter].dim = ordered_moves[i][0][1];
            rearrange_moves[counter].startingConfig = ColumnAt(Matrix,ordered_moves[i][0][1]);
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
            Matrix[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
            rearrange_moves[counter].endingConfig = ColumnAt(Matrix,ordered_moves[i][0][1]);
            counter ++;
        }
        if(ordered_moves[i][0][1] != ordered_moves[i][1][1] && ordered_moves[i][0][0] != ordered_moves[i][1][0]){
                //diagonal move to be split up
                //col move

            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = false;
            rearrange_moves[counter].dim = ordered_moves[i][0][0];
            rearrange_moves[counter].startingConfig = ColumnAt(Matrix,ordered_moves[i][0][1]);
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][1][1]] = true;
            rearrange_moves[counter].endingConfig = ColumnAt(Matrix,ordered_moves[i][0][1]);
            counter ++;

            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = false;
            rearrange_moves[counter].dim = ordered_moves[i][0][1];
            rearrange_moves[counter].startingConfig = Matrix[ordered_moves[i][0][0]];
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][1][1]] = false;
            Matrix[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
            rearrange_moves[counter].endingConfig = Matrix[ordered_moves[i][0][0]];
            counter ++;
        }
    }
    return rearrange_moves;
}



//////////////////////////END///////////////////////////////////////////////


vector<RearrangementMove> rearrange(vector<vector<bool>> Array, rearrange_method method,rearrange_mode mode)
{

    if(method == BALANCE_COMPRESS){
      return BalanceCompressAlg(Array,mode);
    }

    if(method == HUNGARIAN){
      return compute(Array,mode);
    }

}
