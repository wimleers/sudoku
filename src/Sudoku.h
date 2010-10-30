// $Id: Sudoku.h 297 2008-05-30 20:16:20Z wimleers $


/**
 * Sudoku class definition.
 * 
 * @file Sudoku.h
 * @author Bram Bonne
 */


#ifndef SUDOKU_H
#define SUDOKU_H


#include "InternalException.h"
#include "Board.h"
#include "PositionElement.h"
#include <list>
#include <stack>
#include <time.h>
#include <stdlib.h>
using namespace std;


class Sudoku {
public:
    Sudoku(void) { }
    virtual ~Sudoku(void) { }

    static void GenerateBoard(Board * board, int level = 1);
    static bool SolveBoard(Board * board);
    static bool BoardIsSolvable(Board board, bool scanSolveOnly = true);
    static int NumLevels(void) { return 5; }
    
private:
    static bool ScanSolve(Board* board);
    static bool BackTrackSolve(Board* board, int startx = 0, int starty = 0);
    static void GenerateRandomSolution(Board* board);
    static bool SolvableWithRemoval(Board* board, int x, int y);
    static int LevelToNumMoves(int level);
    static bool BoardHasFilledParts(Board* board);
};

#endif //SUDOKU_H
