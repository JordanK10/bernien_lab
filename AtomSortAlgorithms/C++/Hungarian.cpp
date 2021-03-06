#include "Hungarian.h"

using namespace std;

Hungarian::Hungarian(int dim, float prob)
{
    Array = MakeBoolArray(dim,prob);
}

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

tuple<vector<bool>,vector<bool>> Hungarian::clear_covers(int n){
    vector<bool> a = {};
    vector <bool> b = {};
    for(int i = 0; i < n; i ++){
        a.push_back(false);
        b.push_back(false);
    }
    return make_tuple(a,b);
}



bool Hungarian::Check(vector<vector<bool>> Array, vector<int> RowRange, vector<int> ColRange){
    int i = RowRange[0];
    int j = ColRange[0];
    bool check = true;
    while(i<RowRange[1]){
        while(j<ColRange[1]){
            if(Array[i][j] != true){
                check = false;
            }
            j ++;
        }
        j = ColRange[0];
        i ++;
    }
    return check;
}

vector<vector<bool>> Hungarian::Move(vector<vector<bool>> Array, vector<int> pos1,vector<int> pos2){
    vector<vector<bool>> NewArray = Array;
    NewArray[pos1[0]][pos1[1]] = false;
    NewArray[pos2[0]][pos2[1]] = true;
    return NewArray;
}

tuple<int,int> Hungarian::find_zero(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered){
            signed int row = -1;
            signed int col = -1;
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
            return make_tuple(row, col);
        }

int Hungarian::find_star_row(vector<vector<int>> marked, int row, int n){
        signed int col = -1;
        for(int j = 0; j<n;j++){
            if(marked[row][j] == 1){
                col = j;
                break;
            }
        }
        return col;
}

int Hungarian::find_star_col(vector<vector<int>> marked, int col, int n){
        signed int row = -1;
        for(int j = 0; j<n;j++){
            if(marked[j][col] == 1){
                row = j;
                break;
            }
        }
        return row;
}

int Hungarian::find_prime_row(int n, int row, vector<vector<int>> marked){
        signed int col = -1;
        for(int j = 0;j<n;j++){
            if(marked[row][j] == 2){
                col = j;
                break;
            }
        }
        return col;
}

vector<vector<int>> Hungarian::erase_primes(int n, vector<vector<int>> marked){
        for(int i = 0; i<n; i ++){
            for(int j = 0; j<n;j++){
                if(marked[i][j] == 2){
                    marked[i][j] = 0;
                }
            }
        }
    return marked;
}                int atoms = 0;

vector<vector<int>> Hungarian::convert_path(int counter, vector<vector<int>> path, int n, vector<vector<int>> marked){
    for(int i = 0; i<counter + 1; i++){
        if(marked[path[i][0]][path[i][1]] == 1){
            marked[path[i][0]][path[i][1]] = 0;
        }else{
            marked[path[i][0]][path[i][1]] = 1;
        }
    }
    return marked;
}

int Hungarian::find_smallest(vector<vector<int>> CostMatrix, int n, vector<bool> row_covered, vector<bool> col_covered){
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
int Hungarian::metric(vector<int> pos1, vector<int> pos2){
    signed int x = pos1[0] - pos2[0];
    signed int y = pos1[1] - pos2[1];
    if(x<0){
        x = -x;
    }
    if(y<0){
        y = -y;
    }
    int distance = x*x + y*y;
    return distance;
}

tuple<vector<vector<int>>,vector<vector<int>>, int> Hungarian::ToCostMatrix(vector<vector<bool>> InitArray, vector<vector<int>> TargetArray, int Dim1){
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
                int dist = metric(InitPositions[j],TargetArray[i]);
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

tuple<vector<vector<int>>, vector<int>, vector<int>> Hungarian::ToTargetArray(int dim, vector<float> COM){
    float x = dim;
    x -= 1;
    x /= 2;
    vector<int> RowRange = {COM[0] - x, COM[0] + x};
    int r0 = RowRange[0];
    int r1 = RowRange[1];
    if(RowRange[0] - r0 > 0.5){
        r0 += 1;
    }
    if(RowRange[1] - r1 > 0.5){
        r1 += 1;
    }
    RowRange = {r0,r1};

    vector<int> ColRange = {COM[1] - x, COM[1] + x};
    int c0 = ColRange[0];
    int c1 = ColRange[1];
    if(ColRange[0] - r0 > 0.5){
        c0 += 1;
    }
    if(ColRange[1] - r1 > 0.5){
        c1 += 1;
    }
    ColRange = {c0,c1};

    vector<vector<int>> TargetArray = {};
    int i = RowRange[0];
    int j = ColRange[0];
    while(i <= RowRange[1]){
        while(j <= ColRange[1]){
            TargetArray.push_back({i,j});
            j+= 1;
        }
        i += 1;
        j = ColRange[0];
    }
    return make_tuple(TargetArray,RowRange,ColRange);
}

vector<float> Hungarian::CenterOfMass(vector<vector<bool>> Array, int dim){
        int i = 0;
        int j = 0;
        float RowWeight = 0.0;
        float ColWeight = 0.0;
        float TotalWeight = 0.0;
        while(i < dim){
            while(j<dim){
                if(Array[i][j] == true){
                    RowWeight += i;
                    ColWeight += j;
                    TotalWeight += 1;
                }
                j += 1;
            }
            j = 0;
            i ++;
        }
        RowWeight /= TotalWeight;
        ColWeight /= TotalWeight;
        return {RowWeight,ColWeight};
}

vector<vector<vector<int>>> Hungarian::interpret_results(vector<vector<int>> InitPositions, vector<vector<int>> TargetArray, vector<vector<int>> results){
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

vector<vector<vector<int>>> Hungarian::order(vector<vector<vector<int>>> moves, vector<vector<bool>> Array){
        int i = 0;
        int n = moves.size();
        vector<int> todo = {};
        vector<int> todo1 = {};
        int m;
        vector<vector<vector<int>>> properorder = {};
        while(i < n){
            if(Array[moves[i][1][0]][moves[i][1][1]] == false){
                Array = Move(Array,moves[i][0],moves[i][1]);
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
                    Array = Move(Array,moves[j][0],moves[j][1]);
                    properorder.push_back(moves[j]);
                }else{
                    todo.push_back(j);
                }
            }
            k = todo.size();
            if(k == 0){
                break;
            }
        }
    return properorder;
}



tuple<float, vector<vector<vector<int>>>, bool> Hungarian::compute(vector<vector<bool>> Matrix,int TargetDim){
            auto start = chrono::steady_clock::now();
            int ArrayDim = Matrix.size();
            int atoms = 0;
            if(TargetDim == 0){
                for(int i = 0;i<ArrayDim;i++){
                    for(int j = 0;j<ArrayDim;j++){
                        if(Matrix[i][j] == true){
                            atoms ++;
                        }
                    }
                }
            TargetDim = sqrt(atoms);
            }
            vector<float> COM = CenterOfMass(Matrix,ArrayDim);
            vector<vector<int>> TargetArray;
            vector<int> RowRange;
            vector<int> ColRange;
            tie(TargetArray,RowRange,ColRange) = ToTargetArray(TargetDim, COM);

            vector<vector<int>> CostMatrix;
            vector<bool> row_covered;
            vector<bool> col_covered;
            int n;
            int Z0_r = 0;
            int Z0_c = 0;
            vector<vector<int>> marked;
            vector<vector<int>> path;
            int diff;
            vector<vector<int>> InitPositions;
            tie(CostMatrix,InitPositions, diff) = ToCostMatrix(Matrix,TargetArray,ArrayDim);
            n = CostMatrix.size();
            tie(row_covered,col_covered) = clear_covers(n);
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
                case 1: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step1(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 2: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step2(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 3: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step3(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 4: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step4(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 5: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step5(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
                    break;
                case 6: tie(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r) = step6(n, CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
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
        auto End = chrono::steady_clock::now();

        for(i = 0;i<ordered_moves.size();i++){
            Matrix = Move(Matrix,ordered_moves[i][0],ordered_moves[i][1]);
        }


        bool check = Check(Matrix,RowRange, ColRange);
        float duration = chrono::duration_cast<chrono::microseconds>(End - start).count();
        return make_tuple(duration,ordered_moves, check);
}

int Hungarian::minfind(vector<int> a){
            int n = a.size();
            int minval = a[0];
            for(int i = 0; i < n;i++){

                if(a[i]<minval){
                    minval = a[i];
                }
            }
            return minval;
            }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> Hungarian::step1(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
            int minval;
            for(int i = 0;i<n;i++){
                minval = minfind(CostMatrix[i]);
                for(int j = 0; j < n;j++){
                    CostMatrix[i][j] -= minval;
                }
            }
        return make_tuple(2,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> Hungarian::step2(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
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


        tie(row_covered,col_covered) = clear_covers(n);
        return make_tuple(3,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> Hungarian::step3(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
        int step;
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
        return make_tuple(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> Hungarian::step4(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
        int step = 0;
        bool done = false;
        signed int row = -1;
        signed int col = -1;
        signed int star_col = -1;
        while(done == false){
            tie(row,col) = find_zero(n, CostMatrix,row_covered,col_covered);
            if(row<0){
                done = true;
                step = 6;
            }else{
                marked[row][col] = 2;
                star_col = find_star_row(marked, row, n);
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
        return make_tuple(step,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> Hungarian::step5(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
        int counter = 0;
        path[counter][0] = Z0_r;
        path[counter][1] = Z0_c;
        signed int row;
        signed int col;
        bool done = false;
        while(done == false){
            row = find_star_col(marked,path[counter][1], n);
            if(row>= 0){
                counter += 1;
                path[counter][0] = row;
                path[counter][1] = path[counter - 1][1];
            }else{
                done = true;
            }
            if(done == false){
                col = find_prime_row(n, path[counter][0], marked);
                counter += 1;
                path[counter][0] = path[counter-1][0];
                path[counter][1] = col;
            }
        }
        marked = convert_path(counter, path, n, marked);
        tie(row_covered, col_covered) = clear_covers(n);
        marked = erase_primes(n, marked);
        return make_tuple(3,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
        }

tuple<int, vector<vector<int>>, vector<bool>, vector<bool>, vector<vector<int>>, vector<vector<int>>, int, int> Hungarian::step6(int n, vector<vector<int>> CostMatrix, vector<bool> row_covered, vector<bool> col_covered, vector<vector<int>> marked, vector<vector<int>> path, int Z0_c, int Z0_r){
        int minval = find_smallest(CostMatrix, n, row_covered, col_covered);
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
        return make_tuple(4,CostMatrix,row_covered,col_covered,marked,path,Z0_c,Z0_r);
}


