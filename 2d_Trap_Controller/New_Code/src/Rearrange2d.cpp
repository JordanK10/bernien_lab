#include "Rearrange2d.h"

using namespace std;

int i;
int j;
int k;
int COM[2];
int timeOut;
vector<vector<int>> bankLocations;
int RowRange1;
int RowRange2;
int ColRange1;
int ColRange2;

int moveCheck(vector<bool> starting, vector<bool> ending);

void setTimeOut(){
    cin >> timeOut;
}

vector<int> one_d_rearrange(vector<bool> start, vector<bool> ending){
    vector<int> endingConfig;
    vector<int> orderedEndingSites;
    for(int i = 0;i<ending.size();i++){
        if(ending[i]){
            orderedEndingSites.push_back(i);
        }
    }
    int j = 0;
    for(int i = 0;i<start.size();i++){
        if(start[i] && j<orderedEndingSites.size()){
            endingConfig.push_back(orderedEndingSites[j]);
            j ++;
        }else{
            endingConfig.push_back(-1);
        }
    }
    return endingConfig;
}

void convert_ending_config(vector<RearrangementMove> &moves,vector<vector<bool>> endings){
    for(int i = 0;i<moves.size();i++){
        moves[i].endingConfig = one_d_rearrange(moves[i].startingConfig,endings[i]);
    }
}

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
}


//////////////////BALANCE_COMPRESS////////////////////////////


//Select a column of the array and return it as a vector
vector<bool> ColumnAt(vector<vector<bool>> Array, int dim){
    vector<bool> new_col;
    for(k=0;k<Array.size();k++)
        if(Array[k][dim]){
            new_col.push_back(true);
        }else{
            new_col.push_back(false);
        }
  return new_col;
}

//compress the number atoms in a row into a specified range, leaving the extras on the edges
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
    while(dim - extras>j){
        newRow[j] = false;
        j++;
    }
    while(j<dim){
        newRow[j] = true;
        j++;
    }
    return newRow;
}

//sum the atoms in the array by row, 0th index is the total atoms in the array
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

//same as rowsum but for the columns
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

//this takes a range of rows and divides it in half,
//then moves atoms up and down until sufficient atoms
//are in each sub-range. returns the new sub-ranges to be balanced
vector<vector<int>> Balance(vector<vector<bool>> &Array, vector<int> &Range, int SufficientAtoms, vector<int> &RowTotal){
    int dim = Array[0].size();
    int center;

    //find the center of the range to the nearest index, rounded down
    //to the nearest integer

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

    //sum the number of atoms in the lower and upper sub-ranges

    while(i <= center){
        Lower += RowTotal[i+1];
        i ++;
    }
    while(i <= Range[1]){
        Upper += RowTotal[i+1];
        i ++;
    }

    //if the total number of atoms is less than the sufficient number needed
    //we have a problem - were trying to make to large a target array

    if(SuffUpper + SuffLower > Upper + Lower){
      cout << "tenemos un problema" << endl;
    }

    //this loop moves atoms up and down until the sufficient number of atoms
    // is <= the number of atoms in each sub-range
    //if sufflower<lower, it looks for the first column with an empty site in the bottom half, and a full site
    //the upper half, beginning with the first col and progressing forward
    //if suffupper<upper it does the opposite process
    //i is the row index, j is the col index, k is a second row index used to search loaded sites after an empty has
    //already been found to move into
    //movefrom and moveto store the loaded and empty sites, respectively

    while(SuffUpper > Upper || SuffLower > Lower){
        i = center + 1;
        vector<signed int> moveto = {-1,-1};
        vector<signed int> movefrom = {-1,-1};
        j = 0;
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

    //-1,-1 is the base state of movefrom and moveto, so if they do not equal -1, i.e. a move has been identified,
    //it is pushed to the moves vector, and then the loop repeats to see if a second move is needed for balancing

    if(moveto[0] != -1 || movefrom[0] != -1){
    Array[movefrom[0]][movefrom[1]] = false;
    Array[moveto[0]][moveto[1]] = true;
    RowTotal[movefrom[0]+1] --;
    RowTotal[moveto[0]+1] ++;
    }
}
//return the new subranges that have just been balanced, to iterate through this process with them until each subrange is a
//single row
return {{Range[0],center},{center + 1,Range[1]}};
}

//maps col into a specified column of Array
vector<vector<bool>> assignCol(vector<vector<bool>> Array, vector<bool> col, int index)
{
    vector<vector<bool>> newArray = Array;
    for(int t = 0;t<Array.size();t++){
        if(col[t]){
            newArray[t][index] = true;
        }else{
            newArray[t][index] = false;
        }
    }
    return newArray;
}

//finds the center of mass of the array
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

bool eq(vector<bool> starting, vector<bool> ending){
    bool done = true;
    int s = starting.size();
    int e = ending.size();
    if(s != e){
        return false;
    }
    for(int t = 0; t<s; t++){
        if(starting[t] != ending[t]){
            done = false;
            break;
        }
    }
    return done;
}

//master alg for the balance-compress function
vector<RearrangementMove> BalanceCompressAlg(vector<vector<bool>> &Array, int mode){
    int g_counter = 0;
    vector<vector<bool>> startArray = Array;
    vector<RearrangementMove> moves;
    vector<vector<bool>> endings;
    int ArrayDim = Array.size();
    vector<int> RowTotals = RowSum(Array);
    int atoms = RowTotals[0];
    if(mode == REC_CENT || mode == REC_LEFT || mode == REC_RIGHT){
        if(ArrayDim*ArrayDim>atoms){
            cout << "array size: " << ArrayDim << endl;
            cout << "too few: only " << atoms << " atoms." << endl;
            return moves;
        }
    }

    int TargetDim = sqrt(atoms);

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
    if(mode == REC_RIGHT){
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

    //this iterates through all the columns, compressing them until there are enough atoms in the spcified row range to
    //run the balance function. This only happens for a standard, and not rectangular, BC since the rectangular row range
    //is the whole hight of the array
 if(mode == CENTER_COM||mode == UL_CORNER||mode == UR_CORNER||mode == LL_CORNER||mode == LR_CORNER||mode == CLOSE_CORNER){
        for(int z=0; z<ArrayDim; z++){
            atoms = 0;
            for(j = row_y; j<=row_z; j++){
                atoms += RowTotals[j+1];
            }
            if(atoms>=TargetDim*TargetDim){
                check = true;
                break;
            }
            tempCol = ColumnAt(Array,z); // Extract desired column
            compressedCol = CompressRow(tempCol,row_y,row_z,ColTotals[z+1]);
            Array = assignCol(Array,compressedCol,z);

            // Push back column and compressed column
            RowTotals = RowSum(Array); // Recalculate row totals
        }
        if(check == false){
            TargetDim --;
            if(mode == UR_CORNER){
                col_y ++;
                row_z --;
            }
            if(mode == UL_CORNER){
                col_z --;
                row_z --;
            }
            if(mode == LR_CORNER){
                col_y ++;
                row_y ++;
            }
            if(mode == LL_CORNER){
                col_z --;
                row_y ++;
            }
        }
    }

    RowRange1 = row_y;
    RowRange2 = row_z;
    ColRange1 = col_y;
    ColRange2 = col_z;

    int s = 0;
    vector<vector<int>> Range1(2);
    int balancedRows = 0;


    //initialize the first rowrange to be balanced - the whole array
    vector<vector<int>> RowRange = {{row_y,row_z}};

    //balance rows until #of balanced rows = target dim
    while(balancedRows < TargetDim){
        Range1 = Balance(Array,RowRange[s],TargetDim,RowTotals);
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

    for(int s = 0;s<Array[0].size();s++){
        vector<bool> start = ColumnAt(startArray,s);
        vector<bool> ending = ColumnAt(Array,s);
        if(!eq(start,ending)){
            moves.push_back(RearrangementMove());
            moves[g_counter].startingConfig = start;
            moves[g_counter].dim = s;
            moves[g_counter].row = false;
            endings.push_back(ending);
            g_counter ++;
        }
    }


    //compress all the rows, store the moves
    i = RowRange[0][0];
    while(i <= RowRange[0][1]){
        moves.push_back(RearrangementMove());
        moves[g_counter].dim = i;
        moves[g_counter].row = true;
        moves[g_counter].startingConfig = Array[i];
        endings.push_back(CompressRow(Array[i],col_y,col_z,RowTotals[i + 1]));

        Array[i] = CompressRow(Array[i],col_y,col_z,RowTotals[i + 1]);

        i++;
        g_counter++;
    }
    vector<RearrangementMove> bankMoves;
    if(mode == REC_LEFT || mode == REC_RIGHT || mode == REC_CENT){
        bankMoves = rectBank(Array);
    }else{
        if(mode != CENTER_COM){
            bankMoves = bank(Array);
        }else{
            cout << "Cannot Bank Centered Array" << endl;
      }
    }

    convert_ending_config(moves,endings);
    moves.insert(moves.end(),bankMoves.begin(),bankMoves.end());
return moves;
}


///////////////////////////HUNGARIAN///////////////////////////////////////


//clears the covered rows and columns
void clear_covers(vector<bool> &a, vector<bool>&b){
    int n = a.size();
    for(int i = 0; i < n; i ++){
        a[i] = false;
        b[i] = false;
    }
}

void seperate(vector<vector<vector<int>>> &moves){
    int a  = moves.size();
    for(int s = 0;s<a;s++){
        if(moves[s][0][0] != moves[s][1][0] || moves[s][0][1] != moves[s][1][1]){
            vector<int> temp = moves[s][1];
            moves[s][1] = {moves[s][0][0],moves[s][1][1]};
            moves.push_back({{moves[s][0][0],moves[s][1][1]},temp});
        }
    }
}

//find a zero that is not in the covered rows/cols
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

//find a marked site in a specified row, return the col
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

//find a marked site in a specified col, return the row
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

//find a primed site in a specified row, return the col
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

//clear all the primes from the marked matrix
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

//convert a path through marked to its opposite value 1->0, and 0->1
void convert_path(int counter, vector<vector<int>> path, vector<vector<int>> &marked){
    for(int i = 0; i<counter + 1; i++){
        if(marked[path[i][0]][path[i][1]] == 1){
            marked[path[i][0]][path[i][1]] = 0;
        }else{
            marked[path[i][0]][path[i][1]] = 1;
        }
    }
}

//find the smallest value not in the marked rows and cols of the cost matrix
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

//define distance on the array
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


//converts a target array and initial array into a cost matrix, using the defined metric function
//returns the new cost matrix
void ToCostMatrix(vector<vector<bool>> InitArray, vector<vector<int>> TargetArray, int Dim1,vector<vector<int>> &InitPositions,vector<vector<int>> &CostMatrix){
    int NumberOfPositions = TargetArray.size();
    int i = 0;
    int j = 0;
    int atoms = 0;
    if(InitPositions.size() == 0){
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
    }else{
        atoms = InitPositions.size();
    }
    i = 0;
    j = 0;
    vector<int> row = {};
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
}

//creates a list of target array sites based upon the row and col ranges specified by the mode
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

//this converts the list of highlighted sites in the cost matrix into moves
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

//this orders the moves so that we are always moving from loaded positions into empty ones
//it prevents the possibility that we we have a case of:
//  0110
//if we want to move 2->3 and 3->4, the end goal is:
//  0011
//but if we do 2->3 first, we will have two atoms in the same trap at one point in time, ejecting both
// this would be bad, so the moves are ordered
vector<vector<vector<int>>> order(vector<vector<vector<int>>> moves, vector<vector<bool>> Array){
        int i = 0;
        int n = moves.size();
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
                if(Array[moves[j][1][0]][moves[j][1][1]] == false && Array[moves[j][0][0]][moves[j][0][1]] == true){
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

//return the lowest value in a vector
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

int minfind1(vector<int> a){
    int n = a.size();
    int index = 0;
    int minval = a[0];
    for(int i = 0; i < n;i++){
        if(a[i]<minval){
            minval = a[i];
            index = i;
        }
    }
    return index;
}

// first step of the hungarian:
//find the lowest entry in each row, and subtract it from that row
// goto step 2
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

//step 2:
//mark every zero not in a covered col or row, and then cover that col and row
//clear covers afterwards
// goto step 3
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

//step 3:
//cover all the rows with a marked site
//if there are n cols, we have 0s in the costmatrix, and we are done
//otherwise, go to step 4
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


//step 4:
//find zeros in the costmatrix not in the covered cols/rows, and store in row/col
//if there are none, proceed to step 6
//otherwise star the zero and find a star in the row
//cover thisrow and uncover the col
//go to step 5
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

//step 5:
//build a path through marked of starred rows/cols
//then convert the path
//the goal of step 3,4,5 is to cover all the zeros of the costmatrrix with the fewest lines possible
//i.e., if we have the matrix:
// 2 0 2
// 1 1 1
// 0 1 1
// this can be covered with a vertical line through the first col and a horizontal line through the first row
// this is 2 lines < 3 = dim of the array, so the algorithm is not done
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
//step 6:
//adjust the cost matrix by subtracting the min val from all uncovered sites
// this in  effect, tells us we have find the first x best choices to minimize cost,
// and were going to start again with the sub matrix created by removing all the already determined rows/cols
// the minval is subtracted from this submatrix so that we are able to start with one zero, while keeping the
//relative differences intact.
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


//master function of the hungarian algorithm
vector<RearrangementMove> compute(vector<vector<bool>> &Matrix,rearrange_mode mode){

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
    //modes are the same as BC, and relatively intuitive
    //this only works on square matrices
    //for now
    // in theory, it can work on any
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

    RowRange1 = row_y;
    RowRange2 = row_z;
    ColRange1 = col_y;
    ColRange2 = col_z;


    vector<vector<int>> TargetArray = ToTargetArray(TargetDim, row_y,row_z,col_y,col_z);
    vector<vector<int>> CostMatrix;
    vector<vector<int>> InitPositions; //starting positions in the given array

    //create the cost matrix
    //this is a matrix storing the value of the distance from each initial site to each
    //ending site
    //Because the number of these is usually different, the cost matrix stars rectangular
    //and then gets padded with rows of zeros until it is square (because the hungarian only works on squares)

    ToCostMatrix(Matrix,TargetArray,ArrayDim,InitPositions,CostMatrix);
    int n = CostMatrix.size();
    vector<bool> row_covered(n);
    vector<bool> col_covered(n);
    int Z0_r = 0;
    int Z0_c = 0;
    vector<vector<int>> marked;
    vector<vector<int>> path;
    //clear covers initializes them with zero values.
    clear_covers(row_covered,col_covered);
    int i = 0;
    vector<int> row;
    //create a marked array the size of the cost matrix starting with zero vals
    //in the future, 1 will be marked, 2 will be starred
    for(i = 0; i < n; i++){
        row.push_back(0);
    }
    for(i = 0; i < n; i++){
        marked.push_back(row);
    }
    row = {};
    //same for the path, but n^2 in size now, since there are n^2 possible lattice sites
    //this is super overkill, the path will never get this long, but oh well better safe than sorry
    // actually I should look at this later for optimization, I think the theoretical limit is CostMatrix.size() in length
    for(i = 0; i < n*2; i++){
        row.push_back(0);
    }
    for(i = 0; i < n*2; i++){
        path.push_back(row);
    }
    bool done = false;
    int step = 1;
    vector<vector<int>> results = {};
    //step = 0 means the algorithm has finished
    // the switch runs through until it is done,
    //depending on the step return, the flow of which can
    //be found in the description of each step
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

    //results are stored as marked sites
    //these get converted to a list of indices signaling a desired move
    for(i = 0; i < n; i++){
        for(int j = 0; j<n;j++){
            if(marked[i][j] == 1){
                results.push_back({i,j});
            }
        }
    }

    //interprets and order the results of the hungarian into moves
    vector<vector<vector<int>>> moves = interpret_results(InitPositions, TargetArray, results);
//    seperate(moves);
    vector<vector<vector<int>>> ordered_moves = order(moves, Matrix);

    //convert the moves into vector<rearrangementmoves>
    //moves staying in one col/row are simple
    //moves that are diagonal are split into two moves
    //it shouldn't ever matter which way you go, since if there was
    //an atom in the site directly next to you, it would be more efficient to move that one
    //i.e. suppose you have:
    // 1 1 0
    // 0 0 0
    // 0 0 1
    // you will never have a move from the upper corner into the center, since it is a shorter distance
    //to move the upper middle there, and the algorithm minimizes distance
    int moveNumber = ordered_moves.size();
    int counter  = 0;
    vector<RearrangementMove> rearrange_moves;
    vector<vector<bool>> endings;
    for(i = 0;i<moveNumber;i++){
        if(ordered_moves[i][0][0] == ordered_moves[i][1][0]){
            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = true;
            rearrange_moves[counter].dim = ordered_moves[i][0][0];
            rearrange_moves[counter].startingConfig = Matrix[ordered_moves[i][0][0]];
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
            Matrix[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
            endings.push_back(Matrix[ordered_moves[i][0][0]]);
            counter++;
        }
        if(ordered_moves[i][0][1] == ordered_moves[i][1][1]){
            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = false;
            rearrange_moves[counter].dim = ordered_moves[i][0][1];
            rearrange_moves[counter].startingConfig = ColumnAt(Matrix,ordered_moves[i][0][1]);
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
            Matrix[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
            endings.push_back(ColumnAt(Matrix,ordered_moves[i][0][1]));
            counter++;
        }
        if(ordered_moves[i][0][1] != ordered_moves[i][1][1] && ordered_moves[i][0][0] != ordered_moves[i][1][0]){
                //diagonal move to be split up
                //col move

            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = true;
            rearrange_moves[counter].dim = ordered_moves[i][0][0];
            rearrange_moves[counter].startingConfig = Matrix[ordered_moves[i][0][0]];
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][1][1]] = true;
            endings.push_back(Matrix[ordered_moves[i][0][0]]);
            counter++;

            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = false;
            rearrange_moves[counter].dim = ordered_moves[i][1][1];
            rearrange_moves[counter].startingConfig = ColumnAt(Matrix,ordered_moves[i][1][1]);
            Matrix[ordered_moves[i][0][0]][ordered_moves[i][1][1]] = false;
            Matrix[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
            endings.push_back(ColumnAt(Matrix,ordered_moves[i][1][1]));
            counter++;
        }
    }
    convert_ending_config(rearrange_moves,endings);
    vector<RearrangementMove> bankMoves = bank(Matrix);
    rearrange_moves.insert(rearrange_moves.end(),bankMoves.begin(),bankMoves.end());
    return rearrange_moves;
}


/////////////////////////Atom Drop Method///////////////////////////////////


//go works simply as a part of choose
//cycling through the set of vals and adding them to the end of the current combination
//until it is of the correct length
//this is called recursively, reflecting that combinations run like n!/k!(n-k)!
void go(int offset, int k,vector<vector<int>> &r,vector<int> &setOfVals,vector<int> &combination) {
  if (k == 0) {
    r.push_back(combination);
    return;
  }
  for (int i = offset; i <= setOfVals.size() - k; ++i) {
    combination.push_back(setOfVals[i]);
    go(i+1, k-1,r, setOfVals,combination);
    combination.pop_back();
  }
}

//returns all possible combinations of start of length n
vector<vector<int>> choose(vector<int> start, int n)
{
    vector<int> combination;
    vector<vector<int>> r;
    go(0,n,r,start,combination);
    return r;
}

//appends all the rows and cols with enough atoms
//returns a bool specifying if the algorithm is possible
// with the given array and target dim
bool test(vector<vector<bool>> Array,vector<int> &rows,vector<int> &cols, vector<int> rowTotals, vector<int> colTotals,int n)
{
    rows = {};
    cols = {};
    for(int s = 0;s<Array.size();s++){
        if(rowTotals[s+1]>=n){
            rows.push_back(s);
        }
    }
    for(int s = 0;s<Array.size();s++){
        if(colTotals[s+1]>=n){
            cols.push_back(s);
        }
    }
    if(cols.size()<n || rows.size()<n){
        return false;
    }else{
        return true;
    }
}

//checks to see if the given rows and cols all intersect at atoms, returns bool
//if so, yuo have found a perfect, disjoint array
bool check(vector<vector<bool>> Array,vector<int> rows, vector<int> cols)
{
    bool good = true;
    for(int s = 0;s<rows.size();s++){
        for(int t = 0;t<cols.size();t++){
            if(Array[rows[s]][cols[t]] == false){
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

bool in(vector<int> row, int n)
{
    bool yes = false;
    for(int s = 0;s<row.size();s++){
        if(row[s] == n){
            yes = true;
            break;
        }
    }
    return yes;
}

//main function
vector<RearrangementMove> DropItLikeItsHot(vector<vector<bool>> &Array){
    int g_counter = 0;
    int n = Array.size();
    vector<int> rowTotals = RowSum(Array);
    vector<int> colTotals = ColSum(Array);
    int atoms = rowTotals[0];
    int targetDim = sqrt(atoms);
    vector<int> rows;
    vector<int> cols;
    vector<int> goodRows;
    vector<int> goodCols;
    bool good = false;
    bool done = false;
    auto start = chrono::high_resolution_clock::now();
    bool timedOut = false;

    //cycles though until correct rows/cols have been found to create a perfect, disjoint array

    while(!done){
        if(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() > timeOut){
            timedOut = true;
            break;
        }
        //tests the array with given targetdim, lowers if impossible
        while(1){
            good = test(Array,rows,cols,rowTotals,colTotals,targetDim);
            if(good == true){
                break;
            }else{
                targetDim --;
            }
        }
        if(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() > timeOut){
            timedOut = true;
            break;
        }
        vector<vector<int>> rCombs = choose(rows,targetDim);
        vector<vector<int>> cCombs = choose(cols,targetDim);
        if(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() > timeOut){
            timedOut = true;
            break;
        }
        //checks each row/col combination until it finds a working one,
        //if one is not found, it lowers the target-dim, and starts again
        //if this occurs, this is gonna take a while
        for(int i = 0;i<rCombs.size();i++){
            for(int j = 0;j<cCombs.size();j++){
                done = check(Array, rCombs[i],cCombs[j]);
                if(chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() > timeOut){
                    timedOut = true;
                    break;
                }
                if(done == true){
                    goodRows = rCombs[i];
                    goodCols = cCombs[j];
                    break;
                }
            }
            if(done == true || timedOut == true){break;}
        }
        if(done == false){targetDim --;}
    }
    vector<RearrangementMove> moves;
    if(!timedOut){
        //initialize a nullRow of all zeros
        vector<int> nullRow;
        for(int i = 0;i<Array.size();i++){
            nullRow.push_back(-1);
        }
        //push back null rows for every row to be deleted, leaving just the disjoint array
        for(int i = 0;i<Array.size();i++){
            if(!in(goodRows,i)){
                moves.push_back(RearrangementMove());
                moves[g_counter].dim = i;
                moves[g_counter].row = true;
                moves[g_counter].startingConfig = Array[i];
                moves[g_counter].endingConfig = nullRow;
                g_counter++;
            }
            if(!in(goodCols,i)){
                moves.push_back(RearrangementMove());
                moves[g_counter].dim = i;
                moves[g_counter].row = false;
                moves[g_counter].startingConfig = ColumnAt(Array,i);
                moves[g_counter].endingConfig = nullRow;
                g_counter ++;
            }
        }
    }else{
        moves.push_back(RearrangementMove());
        moves[0].dim = -1;
    }

    return moves;
}


//////////////////////////////////BANKING AND REPLENSIHING///////////////////////////////////////
bool detectVacancies(vector<vector<int>> &vacant,vector<vector<bool>> Array)
{
    for(i = 0;i<Array.size();i++){
        for(j = 0;j<Array[0].size();j++){
            if(!Array[i][j] && i>=RowRange1 && i <= RowRange2 && j>=ColRange1 && j<= ColRange2){
                vacant.push_back({i,j});
            }
        }
    }


    for(int m = 0; m<bankLocations.size(); m++){
        if(!Array[bankLocations[m][0]][bankLocations[m][1]]){
            bankLocations.erase(bankLocations.begin() + m);
        }
    }
    if(vacant.size() == 0){
        return false;
    }else{
        return true;
    }
}

void findExtras(vector<vector<bool>> Array, vector<vector<int>> &atoms){
    for(int s = 0; s<Array.size();s++){
        for(int m = 0; m<Array[0].size(); m++){
            if(s>RowRange2 || s<RowRange1 || m<ColRange1 || m>ColRange2){
                if(Array[s][m] == true){
                    atoms.push_back({s,m});
                }
            }
        }
    }
}

vector<RearrangementMove> bank(vector<vector<bool>> &Array){
    vector<RearrangementMove> bankMoves;
    vector<RearrangementMove> moves;
    int counter = 0;
    vector<vector<int>> positions;

    int corner;
    if(RowRange1 == 0){
        if(ColRange1 == 0){
            corner = 1; //UL
        }else{
            corner = 2; //UR
        }
    }else{
        if(ColRange1 == 0){
            corner = 3; //LL
        }else{
            corner = 4; //LR
        }
    }

    vector<vector<int>> path;
    bool toggle = false;
    if(corner == 1){
        j = 0;
        i = Array.size() - 1;
    }
    if(corner == 2){
        i = Array.size() - 1;
        j = Array[0].size() - 1;
    }
    if(corner == 3){
        i = 0;
        j = 0;
    }
    if(corner == 4){
        i = 0;
        j = Array[0].size()-1;
    }

    while(1){
        path.push_back({i,j});
        if(corner == 1 || corner == 3){
            if(toggle){
                j--;
            }else{
                j++;
            }
        }
        if(corner == 2 || corner == 4){
            if(toggle){
                j++;
            }else{
                j--;
            }
        }

        if(corner == 1 && j>ColRange2){
            toggle = true;
            i--;
            j--;
        }
        if(corner == 1 && j<0){
            toggle = false;
            j++;
            i--;
        }

        if(corner == 2 && j<ColRange1){
            toggle = true;
            i--;
            j++;
        }
        if(corner == 2 && j>Array[0].size() - 1){
            toggle = false;
            j--;
            i--;
        }

        if(corner == 3 && j<0){
            toggle = false;
            i++;
            j++;
        }
        if(corner == 3 && j>ColRange2){
            toggle = true;
            j--;
            i++;
        }

        if(corner == 4 && j<ColRange1){
            toggle = true;
            i++;
            j++;
        }
        if(corner == 4 && j>Array[0].size() - 1){
            toggle = false;
            j--;
            i++;
        }
        if(corner == 1 || corner == 2){
            if(i == RowRange2){
                break;
            }
        }
        if(corner == 3 || corner == 4){
            if(i == RowRange1){
                break;
            }
        }
    }

    if(corner == 1){
        for(int s = ColRange2 + 1;s<Array[0].size();s++){
            for(int t = Array.size()-1;t>=0;t--){
                path.push_back({t,s});
            }
        }
    }

    if(corner == 2){
        for(int s = ColRange1-1;s>=0;s--){
            for(int t = Array.size() - 1;t>=0;t--){
                path.push_back({t,s});
            }
        }
    }

     if(corner == 3){
        for(int s = ColRange2+1;s<Array[0].size();s++){
            for(int t = 0;t<Array.size();t++){
                path.push_back({t,s});
            }
        }
    }

    if(corner == 4){
        for(int s = ColRange1 - 1;s>=0;s--){
            for(int t = 0;t<Array.size();t++){
                path.push_back({t,s});
            }
        }
    }

    vector<vector<vector<int>>> ordered_moves;

    int j = 0;
    for(int i = 0;i<path.size();i++){
        if(Array[path[i][0]][path[i][1]]){
            ordered_moves.push_back({path[i],path[j]});
            bankLocations.push_back(path[j]);
            j++;
        }
    }

    vector<vector<bool>> endings;
    int n = ordered_moves.size();
    for(i = 0;i<n;i++){
    if(!Array[ordered_moves[i][1][0]][ordered_moves[i][1][1]]){
        if(ordered_moves[i][0][0] == ordered_moves[i][1][0]){
            moves.push_back(RearrangementMove());
            moves[counter].row = true;
            moves[counter].dim = ordered_moves[i][0][0];
            moves[counter].startingConfig = Array[ordered_moves[i][0][0]];
            Array[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
            Array[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
            endings.push_back(Array[ordered_moves[i][0][0]]);
            counter ++;

        }
        if(ordered_moves[i][0][1] == ordered_moves[i][1][1]){
            moves.push_back(RearrangementMove());
            moves[counter].row = false;
            moves[counter].dim = ordered_moves[i][0][1];
            moves[counter].startingConfig = ColumnAt(Array,ordered_moves[i][0][1]);
            Array[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
            Array[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
            endings.push_back(ColumnAt(Array,ordered_moves[i][0][1]));
            counter ++;

        }
        if(ordered_moves[i][0][1] != ordered_moves[i][1][1] && ordered_moves[i][0][0] != ordered_moves[i][1][0]){
                //diagonal move to be split up
            //col move first
            if(Array[ordered_moves[i][1][0]][ordered_moves[i][0][1]] == false){
                moves.push_back(RearrangementMove());
                moves[counter].row = false;
                moves[counter].dim = ordered_moves[i][0][1];
                moves[counter].startingConfig = ColumnAt(Array,ordered_moves[i][0][1]);
                Array[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
                Array[ordered_moves[i][1][0]][ordered_moves[i][0][1]] = true;
                endings.push_back(ColumnAt(Array,ordered_moves[i][0][1]));
                counter ++;

                //row move second
                moves.push_back(RearrangementMove());
                moves[counter].row = true;
                moves[counter].dim = ordered_moves[i][1][0];
                moves[counter].startingConfig = Array[ordered_moves[i][1][0]];
                Array[ordered_moves[i][1][0]][ordered_moves[i][0][1]] = false;
                Array[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
                endings.push_back(Array[ordered_moves[i][1][0]]);
                counter ++;
            }
            if(Array[ordered_moves[i][0][0]][ordered_moves[i][1][1]] == false){
                //row first
                moves.push_back(RearrangementMove());
                moves[counter].row = true;
                moves[counter].dim = ordered_moves[i][0][0];
                moves[counter].startingConfig = Array[ordered_moves[i][0][0]];
                Array[ordered_moves[i][0][0]][ordered_moves[i][0][1]] = false;
                Array[ordered_moves[i][0][0]][ordered_moves[i][1][1]] = true;
                endings.push_back(Array[ordered_moves[i][0][0]]);
                counter ++;

                //col second
                moves.push_back(RearrangementMove());
                moves[counter].row = false;
                moves[counter].dim = ordered_moves[i][1][1];
                moves[counter].startingConfig = ColumnAt(Array,ordered_moves[i][1][1]);
                Array[ordered_moves[i][0][0]][ordered_moves[i][1][1]] = false;
                Array[ordered_moves[i][1][0]][ordered_moves[i][1][1]] = true;
                endings.push_back(ColumnAt(Array,ordered_moves[i][1][1]));
                counter ++;
            }
            if(Array[ordered_moves[i][0][0]][ordered_moves[i][1][1]] == true && Array[ordered_moves[i][1][0]][ordered_moves[i][0][1]] == true){
                ordered_moves.push_back(ordered_moves[i]);
              //  cout << ordered_moves[i][1][0] << " " << ordered_moves[i][1][1] << endl;
                if(i != n-1){n ++;}else{cout << "prob!" << endl;}
            }
        }
    }
    }
    convert_ending_config(moves,endings);
    return moves;
}

bool Toggle(bool a){
    bool b;
    if(a == false){
        b = true;
    }else{
        b = false;
    }
    return b;
}

vector<RearrangementMove> rectBank(vector<vector<bool>> &Array){
    vector<RearrangementMove> rearrange_moves;
    vector<vector<vector<int>>> moves;
    int Height = Array.size();
    vector<vector<int>> path;
    if(ColRange1 != 0){
        i = 0;
        j = 0;
        bool invert = true;
        while(i<ColRange1){
            while(j<Height && j>-1){
                path.push_back({j,i});
                if(invert == true){
                    j ++;
                }else{
                    j --;
                }
            }
            i ++;
            if(invert == true){
                j --;
            }else{
                j ++;
            }
            invert = Toggle(invert);
        }
    }else{
        i = Array[0].size()-1;
        j = 0;
        bool invert = true;
        while(i>ColRange2){
            while(j<Height && j>-1){
                path.push_back({j,i});
                if(invert == true){
                    j ++;
                }else{
                    j --;
                }
            }
            i --;
            if(invert == true){
                j --;
            }else{
                j ++;
            }
            invert = Toggle(invert);
        }
    }
    i = 0;
    j = 0;
    while(j<path.size()){
        if(Array[path[j][0]][path[j][1]] == true){
            moves.push_back({path[j],path[i]});
            i ++;
        }
        j ++;
    }
    int counter = 0;
    vector<vector<bool>> endings;
    for(int i = 0;i<moves.size();i++){
        if(moves[i][0][0] == moves[i][1][0]){
            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = true;
            rearrange_moves[counter].dim = moves[i][0][0];
            rearrange_moves[counter].startingConfig = Array[moves[i][0][0]];
            Array[moves[i][0][0]][moves[i][0][1]] = false;
            Array[moves[i][1][0]][moves[i][1][1]] = true;
            endings.push_back(Array[moves[i][0][0]]);
            counter++;
        }
        if(moves[i][0][1] == moves[i][1][1]){
            rearrange_moves.push_back(RearrangementMove());
            rearrange_moves[counter].row = false;
            rearrange_moves[counter].dim = moves[i][0][1];
            rearrange_moves[counter].startingConfig = ColumnAt(Array,moves[i][0][1]);
            Array[moves[i][0][0]][moves[i][0][1]] = false;
            Array[moves[i][1][0]][moves[i][1][1]] = true;
            endings.push_back(ColumnAt(Array,moves[i][0][1]));
            counter++;
        }
        if(moves[i][0][1] != moves[i][1][1] && moves[i][0][0] != moves[i][1][0]){
                //diagonal move to be split up
            if(Array[moves[i][0][0]][moves[i][1][1]] == false){
                rearrange_moves.push_back(RearrangementMove());
                rearrange_moves[counter].row = true;
                rearrange_moves[counter].dim = moves[i][0][0];
                rearrange_moves[counter].startingConfig = Array[moves[i][0][0]];
                Array[moves[i][0][0]][moves[i][0][1]] = false;
                Array[moves[i][0][0]][moves[i][1][1]] = true;
                endings.push_back(Array[moves[i][0][0]]);
                counter++;

                rearrange_moves.push_back(RearrangementMove());
                rearrange_moves[counter].row = false;
                rearrange_moves[counter].dim = moves[i][1][1];
                rearrange_moves[counter].startingConfig = ColumnAt(Array,moves[i][1][1]);
                Array[moves[i][0][0]][moves[i][1][1]] = false;
                Array[moves[i][1][0]][moves[i][1][1]] = true;
                endings.push_back(ColumnAt(Array,moves[i][1][1]));
                counter++;
            }else{
                rearrange_moves.push_back(RearrangementMove());
                rearrange_moves[counter].row = false;
                rearrange_moves[counter].dim = moves[i][0][1];
                rearrange_moves[counter].startingConfig = ColumnAt(Array,moves[i][0][1]);
                Array[moves[i][0][0]][moves[i][0][1]] = false;
                Array[moves[i][1][0]][moves[i][0][1]] = true;
                endings.push_back(ColumnAt(Array,moves[i][0][1]));
                counter++;

                rearrange_moves.push_back(RearrangementMove());
                rearrange_moves[counter].row = true;
                rearrange_moves[counter].dim = moves[i][1][0];
                rearrange_moves[counter].startingConfig = Array[moves[i][1][0]];
                Array[moves[i][1][0]][moves[i][0][1]] = false;
                Array[moves[i][1][0]][moves[i][1][1]] = true;
                endings.push_back(Array[moves[i][1][0]]);
                counter++;
            }
        }
    }
    convert_ending_config(rearrange_moves,endings);
    return rearrange_moves;
}

int findNearestBank(vector<vector<bool>> Array, vector<int> vacancy){
    int n = Array.size() + Array[0].size();
    int index = 0;
    vector<int> distance;
    for(int i = 0;i<bankLocations.size();i++){
        distance.push_back((vacancy[0] - bankLocations[i][0])*(vacancy[0] - bankLocations[i][0]) + (vacancy[1] - bankLocations[i][1])*(vacancy[1] - bankLocations[i][1]));
    }
    for(int i = 0;i<distance.size();i++){
        if(distance[i] <= distance[index]){
            index = i;
        }
    }
    return index;
}

vector<RearrangementMove> fillVacancies(vector<vector<bool>> &Array)
{
    vector<RearrangementMove> moves;
    int counter = 0;
    bool done = false;
    int index;
    vector<vector<int>> vacant;
    vector<int> rowTotals = RowSum(Array);
    vector<vector<bool>> endings;
    if(detectVacancies(vacant,Array)){
        for(int m = 0; m<vacant.size(); m++){
            index = findNearestBank(Array, vacant[m]);
            if(bankLocations[index][1] == vacant[m][1]){
                //compress row
                moves.push_back(RearrangementMove());
                moves[counter].dim = vacant[m][1];
                moves[counter].row = false;
                moves[counter].startingConfig = ColumnAt(Array,vacant[m][1]);
                Array[bankLocations[index][0]][bankLocations[index][1]] = false;
                Array[vacant[m][0]][vacant[m][1]] = true;
                endings.push_back(ColumnAt(Array,vacant[m][0]));
                bankLocations.erase(bankLocations.begin() + index);
                counter ++;
            }else{
                //two moves
                moves.push_back(RearrangementMove());
                moves[counter].dim = bankLocations[index][0];
                moves[counter].row = true;
                moves[counter].startingConfig = Array[vacant[m][0]];
                Array[bankLocations[index][0]][bankLocations[index][1]] = false;
                Array[bankLocations[index][0]][vacant[m][1]] = true;
                endings.push_back(Array[vacant[m][0]]);
                counter ++;

                moves.push_back(RearrangementMove());
                moves[counter].dim = vacant[m][1];
                moves[counter].row = false;
                moves[counter].startingConfig = ColumnAt(Array,vacant[m][1]);
                Array[bankLocations[index][0]][vacant[m][1]] = false;
                Array[vacant[m][0]][vacant[m][1]] = true;
                endings.push_back(ColumnAt(Array,vacant[m][1]));
                bankLocations.erase(bankLocations.begin() + index);
                counter ++;
            }
        }
    }
    convert_ending_config(moves,endings);
    return moves;
}




//////////////////////////END///////////////////////////////////////////////

vector<RearrangementMove> rearrange(vector<vector<bool>> &Array, rearrange_method method,rearrange_mode mode)
{
    if(method == BALANCE_COMPRESS){
        return BalanceCompressAlg(Array,mode);
    }

    if(method == HUNGARIAN){
        return compute(Array,mode);
    }
    if(method == DROP_IT_LIKE_ITS_HOT){
        return DropItLikeItsHot(Array);
    }

}

/////////////////////////DONE WITH REARRANGE2D.CPP/////////////////////////////////////////////////
