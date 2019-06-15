"""
The Munkres Class is copyrighted by Brian M. Clapper, see below.
It has been modified to be used for rearrangement.

Copyright and License
=====================

Copyright 2008-2016 Brian M. Clapper

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""


# ---------------------------------------------------------------------------
# Imports
# ---------------------------------------------------------------------------

import sys
import copy

# ---------------------------------------------------------------------------
# Exports
# ---------------------------------------------------------------------------

__all__     = ['Munkres', 'make_cost_matrix']

# ---------------------------------------------------------------------------
# Globals
# ---------------------------------------------------------------------------

# Info about the module
__version__   = "1.0.9"
__author__    = "Brian Clapper, bmc@clapper.org"
__url__       = "http://software.clapper.org/munkres/"
__copyright__ = "(c) 2008 Brian M. Clapper"
__license__   = "Apache Software License"

# ---------------------------------------------------------------------------
# Classes
# ---------------------------------------------------------------------------

class Munkres:
    """
    Calculate the Munkres solution to the classical assignment problem.
    See the module documentation for usage.
    """

    def __init__(self):
        """Create a new instance"""
        self.C = None
        self.row_covered = []
        self.col_covered = []
        self.n = 0
        self.Z0_r = 0
        self.Z0_c = 0
        self.marked = None
        self.path = None

    def pad_matrix(self, matrix, pad_value=0):
        """
        Pad a possibly non-square matrix to make it square.

        :Parameters:
            matrix : list of lists
                matrix to pad

            pad_value : int
                value to use to pad the matrix

        :rtype: list of lists
        :return: a new, possibly padded, matrix
        """
        max_columns = 0
        total_rows = len(matrix)

        for row in matrix:
            max_columns = max(max_columns, len(row))

        total_rows = max(max_columns, total_rows)

        new_matrix = []
        for row in matrix:
            row_len = len(row)
            new_row = row[:]
            if total_rows > row_len:
                # Row too short. Pad it.
                new_row += [pad_value] * (total_rows - row_len)
            new_matrix += [new_row]

        while len(new_matrix) < total_rows:
            new_matrix += [[pad_value] * total_rows]

        return new_matrix

    def compute(self, cost_matrix):
        """
        Compute the indexes for the lowest-cost pairings between rows and
        columns in the database. Returns a list of (row, column) tuples
        that can be used to traverse the matrix.

        :Parameters:
            cost_matrix : list of lists
                The cost matrix. If this cost matrix is not square, it
                will be padded with zeros, via a call to ``pad_matrix()``.
                (This method does *not* modify the caller's matrix. It
                operates on a copy of the matrix.)

                **WARNING**: This code handles square and rectangular
                matrices. It does *not* handle irregular matrices.

        :rtype: list
        :return: A list of ``(row, column)`` tuples that describe the lowest
                 cost path through the matrix

        """
        self.C = self.pad_matrix(cost_matrix)
        self.n = len(self.C)
        self.original_length = len(cost_matrix)
        self.original_width = len(cost_matrix[0])
        self.row_covered = [False for i in range(self.n)]
        self.col_covered = [False for i in range(self.n)]
        self.Z0_r = 0
        self.Z0_c = 0
        self.path = self.__make_matrix(self.n * 2, 0)
        self.marked = self.__make_matrix(self.n, 0)

        done = False
        step = 1

        steps = { 1 : self.__step1,
                  2 : self.__step2,
                  3 : self.__step3,
                  4 : self.__step4,
                  5 : self.__step5,
                  6 : self.__step6 }

        while not done:
            try:
                func = steps[step]
                step = func()
            except KeyError:
                done = True

        # Look for the starred columns
        results = []
        for i in range(self.original_length):
            for j in range(self.original_width):
                if self.marked[i][j] == 1:
                    results += [(i, j)]

        return results

    def __copy_matrix(self, matrix):
        """Return an exact copy of the supplied matrix"""
        return copy.deepcopy(matrix)

    def __make_matrix(self, n, val):
        """Create an *n*x*n* matrix, populating it with the specific value."""
        matrix = []
        for i in range(n):
            matrix += [[val for j in range(n)]]
        return matrix

    def __step1(self):
        """
        For each row of the matrix, find the smallest element and
        subtract it from every element in its row. Go to Step 2.
        """
        C = self.C
        n = self.n
        for i in range(n):
            minval = min(self.C[i])
            # Find the minimum value for this row and subtract that minimum
            # from every element in the row.
            for j in range(n):
                self.C[i][j] -= minval

        return 2

    def __step2(self):
        """
        Find a zero (Z) in the resulting matrix. If there is no starred
        zero in its row or column, star Z. Repeat for each element in the
        matrix. Go to Step 3.
        """
        n = self.n
        for i in range(n):
            for j in range(n):
                if (self.C[i][j] == 0) and \
                        (not self.col_covered[j]) and \
                        (not self.row_covered[i]):
                    self.marked[i][j] = 1
                    self.col_covered[j] = True
                    self.row_covered[i] = True
                    break

        self.__clear_covers()
        return 3

    def __step3(self):
        """
        Cover each column containing a starred zero. If K columns are
        covered, the starred zeros describe a complete set of unique
        assignments. In this case, Go to DONE, otherwise, Go to Step 4.
        """
        n = self.n
        count = 0
        for i in range(n):
            for j in range(n):
                if self.marked[i][j] == 1 and not self.col_covered[j]:
                    self.col_covered[j] = True
                    count += 1

        if count >= n:
            step = 7 # done
        else:
            step = 4

        return step

    def __step4(self):
        """
        Find a noncovered zero and prime it. If there is no starred zero
        in the row containing this primed zero, Go to Step 5. Otherwise,
        cover this row and uncover the column containing the starred
        zero. Continue in this manner until there are no uncovered zeros
        left. Save the smallest uncovered value and Go to Step 6.
        """
        step = 0
        done = False
        row = -1
        col = -1
        star_col = -1
        while not done:
            (row, col) = self.__find_a_zero()
            if row < 0:
                done = True
                step = 6
            else:
                self.marked[row][col] = 2
                star_col = self.__find_star_in_row(row)
                if star_col >= 0:
                    col = star_col
                    self.row_covered[row] = True
                    self.col_covered[col] = False
                else:
                    done = True
                    self.Z0_r = row
                    self.Z0_c = col
                    step = 5

        return step

    def __step5(self):
        """
        Construct a series of alternating primed and starred zeros as
        follows. Let Z0 represent the uncovered primed zero found in Step 4.
        Let Z1 denote the starred zero in the column of Z0 (if any).
        Let Z2 denote the primed zero in the row of Z1 (there will always
        be one). Continue until the series terminates at a primed zero
        that has no starred zero in its column. Unstar each starred zero
        of the series, star each primed zero of the series, erase all
        primes and uncover every line in the matrix. Return to Step 3
        """
        count = 0
        path = self.path
        path[count][0] = self.Z0_r
        path[count][1] = self.Z0_c
        done = False
        while not done:
            row = self.__find_star_in_col(path[count][1])
            if row >= 0:
                count += 1
                path[count][0] = row
                path[count][1] = path[count-1][1]
            else:
                done = True

            if not done:
                col = self.__find_prime_in_row(path[count][0])
                count += 1
                path[count][0] = path[count-1][0]
                path[count][1] = col

        self.__convert_path(path, count)
        self.__clear_covers()
        self.__erase_primes()
        return 3

    def __step6(self):
        """
        Add the value found in Step 4 to every element of each covered
        row, and subtract it from every element of each uncovered column.
        Return to Step 4 without altering any stars, primes, or covered
        lines.
        """
        minval = self.__find_smallest()
        for i in range(self.n):
            for j in range(self.n):
                if self.row_covered[i]:
                    self.C[i][j] += minval
                if not self.col_covered[j]:
                    self.C[i][j] -= minval
        return 4

    def __find_smallest(self):
        """Find the smallest uncovered value in the matrix."""
        minval = sys.maxsize
        for i in range(self.n):
            for j in range(self.n):
                if (not self.row_covered[i]) and (not self.col_covered[j]):
                    if minval > self.C[i][j]:
                        minval = self.C[i][j]
        return minval

    def __find_a_zero(self):
        """Find the first uncovered element with value 0"""
        row = -1
        col = -1
        i = 0
        n = self.n
        done = False

        while not done:
            j = 0
            while True:
                if (self.C[i][j] == 0) and \
                        (not self.row_covered[i]) and \
                        (not self.col_covered[j]):
                    row = i
                    col = j
                    done = True
                j += 1
                if j >= n:
                    break
            i += 1
            if i >= n:
                done = True

        return (row, col)

    def __find_star_in_row(self, row):
        """
        Find the first starred element in the specified row. Returns
        the column index, or -1 if no starred element was found.
        """
        col = -1
        for j in range(self.n):
            if self.marked[row][j] == 1:
                col = j
                break

        return col

    def __find_star_in_col(self, col):
        """
        Find the first starred element in the specified row. Returns
        the row index, or -1 if no starred element was found.
        """
        row = -1
        for i in range(self.n):
            if self.marked[i][col] == 1:
                row = i
                break

        return row

    def __find_prime_in_row(self, row):
        """
        Find the first prime element in the specified row. Returns
        the column index, or -1 if no starred element was found.
        """
        col = -1
        for j in range(self.n):
            if self.marked[row][j] == 2:
                col = j
                break

        return col

    def __convert_path(self, path, count):
        for i in range(count+1):
            if self.marked[path[i][0]][path[i][1]] == 1:
                self.marked[path[i][0]][path[i][1]] = 0
            else:
                self.marked[path[i][0]][path[i][1]] = 1

    def __clear_covers(self):
        """Clear all covered matrix cells"""
        for i in range(self.n):
            self.row_covered[i] = False
            self.col_covered[i] = False

    def __erase_primes(self):
        """Erase all prime markings"""
        for i in range(self.n):
            for j in range(self.n):
                if self.marked[i][j] == 2:
                    self.marked[i][j] = 0

# ---------------------------------------------------------------------------
# Functions
# ---------------------------------------------------------------------------
def metric(pos1,pos2):
    x = pos1[1] - pos2[1]
    y = pos1[0] - pos2[0]
    if x < 0:
        x = -x
    if y < 0:
        y = -y
    distance = x**2+y**2
    return distance

def ToCostMatrix(InitArray,TargetArray,Dim1):
    InitPositions = []
    NumberOfPositions = len(TargetArray)
    i = 0
    j = 0
    atoms = 0
    while i<Dim1:
        while j < Dim1:
            if InitArray[i][j] == True:
                InitPositions.append([i,j])
                atoms += 1
            j += 1
        i += 1
        j = 0
    i = 0
    j = 0
    row = []
    CostMatrix = []
    difference = atoms - NumberOfPositions
    if difference>=0:
        while i < NumberOfPositions:
            while j < atoms:
                row.append(metric(InitPositions[j],TargetArray[i]))
                j += 1
            CostMatrix.append(row)
            j = 0
            i += 1
            row = []
        i = 0
        row = []
        j = 0
        while j < atoms:
            row.append(0)
            j += 1
        while i < difference:
            CostMatrix.append(row)
            i += 1
            

        return [CostMatrix,InitPositions,difference]
    else:
        print "insufficient atoms"
        return[0,0]

def TargetArray(Dim,COM):
    x = float(Dim)
    x -= 1
    x /= 2
    RowRange = [COM[0] - x,COM[0] + x]
    RowRange[0] = round(RowRange[0])
    RowRange[1] = round(RowRange[1])
    RowRange[0] = int(RowRange[0])
    RowRange[1] = int(RowRange[1])

    ColumnRange = [COM[1] - x,COM[1] + x]
    ColumnRange[0] = round(ColumnRange[0])
    ColumnRange[1] = round(ColumnRange[1])
    ColumnRange[0] = int(ColumnRange[0])
    ColumnRange[1] = int(ColumnRange[1])

    TargetArray = []
    i = RowRange[0]
    j = ColumnRange[0]
    while i <= RowRange[1]:
        while j <= ColumnRange[1]:
            TargetArray.append([i,j])
            j += 1
        i += 1
        j = ColumnRange[0]
    
    return [TargetArray,RowRange,ColumnRange]

def CenterOfMass(Array, ArrayDimension):
    i = 0
    j = 0
    RowWeight = 0.0
    ColumnWeight = 0.0
    TotalWeight = 0.0
    while i < ArrayDimension:
        while j < ArrayDimension:
            if Array[i][j] == True:
                RowWeight += i
                ColumnWeight += j
                TotalWeight += 1
            j +=1
        j = 0
        i += 1
    RowWeight /= TotalWeight
    ColumnWeight /= TotalWeight
    return [RowWeight,ColumnWeight]

def Check(Array,RowRange,ColumnRange):
    i = RowRange[0]
    j = ColumnRange[0]
    Check = True
    while i <= RowRange[1]:
        while j <= ColumnRange[1]:
            if Array[i][j] == False:
                Check = False
            j += 1
        i += 1
        j = ColumnRange[0]

    return Check

def Move(Array,position1,position2):
    NewArray = Array
    NewArray[position1[0]][position1[1]] = False
    NewArray[position2[0]][position2[1]] = True
    return NewArray

def Order(Array,moves):
    i = 0
    todo = []
    properorder = []
    while i<len(moves):
        if Array[moves[i][1][0]][moves[i][1][1]] == False:
            Array = Move(Array,moves[i][0],moves[i][1])
            properorder.append(moves[i])
            i += 1
        else:
            todo.append(i)
            i += 1
    while 1:
        todo1 = todo
        todo = []
        for i in todo1:
            if Array[moves[i][1][0]][moves[i][1][1]] == False:
                Array = Move(Array,moves[i][0],moves[i][1])
                properorder.append(moves[i])
            else:
                todo.append(i)
        if len(todo) == 0:
            break
    return properorder

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------
def Hungarian(Array,ArrayDim,TargetDim):
    import datetime

    #start the clock
    start = datetime.datetime.now()

    if TargetDim == 'max':
        import math
        i = 0
        j = 0
        atoms = 0
        while i < ArrayDim:
            while j < ArrayDim:
                if Array[i][j] == True:
                    atoms += 1
                j += 1
            i += 1
            j = 0
        TargetDim = math.sqrt(atoms)
        TargetDim = int(TargetDim)

    COM = CenterOfMass(Array,ArrayDim)

    PlaceHolder = TargetArray(TargetDim,COM)
    Target = PlaceHolder[0]
    RowRange = PlaceHolder[1]
    ColumnRange = PlaceHolder[2]

    PlaceHolder = ToCostMatrix(Array,Target,ArrayDim)
    StartingPositions = PlaceHolder[1]
    CostMatrix = PlaceHolder[0]
    CostMatrixDim = len(CostMatrix)

    m = Munkres()
    Assignments = m.compute(CostMatrix)
    i = 0
    moves = []
    while i < len(Assignments):
        if Assignments[i][0] < len(Target):
            if Target[Assignments[i][0]] != StartingPositions[Assignments[i][1]]:
                moveto = Target[Assignments[i][0]]
                movefrom = StartingPositions[Assignments[i][1]]
                moves.append([movefrom,moveto])
        i += 1
    
    properorder = Order(Array,moves)           

    end = datetime.datetime.now()
    duration = end - start
    fidelity = Check(Array,RowRange,ColumnRange)
    if fidelity == True:
        fidelity = 1
    else:
        fidelity = 0

    return [duration,properorder,fidelity,moves]





