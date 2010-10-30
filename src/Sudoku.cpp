// $Id: Sudoku.cpp 328 2008-06-05 20:40:51Z gooz $

#include "Sudoku.h"
#include <iostream>


//----------------------------------------------------------------------------
// Public methods.

/**
 * Generates a new board.
 *
 * @param level
 *  Difficulty at which we want the board to be solvable
 * @param board
 *  Pointer to memory location where we can store the board
 */
void Sudoku::GenerateBoard(Board * board, int level) {
    bool possibilities;
    list< PositionElement > undoList;
    
    // Generate random solved board
    GenerateRandomSolution(board);
    // Remove elements and push them on the stack until the board is not solvable anymore
    do {
        possibilities = false; // We keep track of any removed elements in the last iteration
        int yoffset = rand() % 9; // So we don't always start erasing the first few elements
        int xoffset = rand() % 9;
        for (int y = 0; y < 9; ++y)
            for (int x = 0; x < 9; ++x) {
                int realx = (x + xoffset) % 9; // The real x and y positions
                int realy = (y + yoffset) % 9;
                if (board->Get(realx, realy) != 0 && SolvableWithRemoval(board, realx, realy)) { // We want the board to be solvable by ScanSolve() only, so it has only 1 solution
                    PositionElement pe(realx, realy, board->Get(realx, realy));
                    undoList.push_back(pe);
                    board->Remove(realx, realy);
                    possibilities = true;
                }
            }
    } while (possibilities);
    
    // Now reinsert/remove as many elements as the current level demands
    int reinsert = LevelToNumMoves(level);
    
    bool stillFoundOne = true;
    stack<PositionElement> redoStack;
    while (reinsert > 0 && stillFoundOne) {
        stillFoundOne = false;
        
        int yoffset = rand() % 9; // So we don't always start erasing the first few elements
        int xoffset = rand() % 9;

        for (int y = 0; y < 9 && reinsert > 0 && !undoList.empty(); ++y)
            for (int x = 0; x < 9 && reinsert > 0 && !undoList.empty(); ++x) {
                int realx = (x + xoffset) % 9; // The real x and y positions
                int realy = (y + yoffset) % 9;
                
                PositionElement pe(realx, realy, board->Get(realx, realy));
                redoStack.push(pe);
                board->Remove(realx, realy); // Remove the element from the board and try to solve it again
                
                bool foundOne = false;
                for (list<PositionElement>::iterator it = undoList.begin(); it != undoList.end() && !foundOne;) {
                    board->Set(it->GetX(), it->GetY(), it->GetE());
                    if (BoardIsSolvable(*board, true)) { // Board is solvable by ScanSolve()
                        PositionElement addableElement(it->GetX(), it->GetY(), it->GetE());
                        redoStack.push(addableElement);
                        
                        it = undoList.erase(it); // Remove from the undoList and add to the redoStack
                        
                        --reinsert;
                        foundOne = true;
                        stillFoundOne = true;
                    }
                    else
                        it++;

                    board->Remove(realx, realy); // Removed in BOTH CASES!
                    
                    if (!foundOne && !redoStack.empty()) {
                        board->Set(redoStack.top().GetX(), redoStack.top().GetY(), redoStack.top().GetE());
                        redoStack.pop();
                    }
                }
            }
    }
    
    if (reinsert > 0 || BoardHasFilledParts(board)) // We don't want the board to contain filled in blocks/rows/columns
        GenerateBoard(board, level); // So start all over again if it happens
    // This is much faster than checking and removing to keep the current level!
    else {
        while (!redoStack.empty()) {
            board->Set(redoStack.top().GetX(), redoStack.top().GetY(), redoStack.top().GetE());
            redoStack.pop();
        }

        // To remove extra elements if level > 5, THIS IS NOT BEING USED AT THIS MOMENT!
        for (int i = 0; i > reinsert; --i) {
            int randx = rand() % 9;
            int randy = rand() % 9;
            if (board->Get(randx, randy) != 0)
                board->Remove(randx, randy);
            else
                ++i;
        }
    }
}

/**
 * Solves the board.
 *
 * @param board
 *  The board we are going to solve
 * @return
 *  Solvable
 */
bool Sudoku::SolveBoard(Board * board) {
    bool solved = false;
    
    if (board->IsValid()) {
        solved = ScanSolve(board);
        if (!solved)
            solved = BackTrackSolve(board);
            // This algorithm will work on a board where the boxes ScanSolve() could solve are already filled in, to reduce overhead
    }
    
    return solved;
}

/**
 * Checks if the board can be solved (standard only with ScanSolve() )
 *
 * @param board
 *  The board we want to check for solvability
 * @param scanSolveOnly
 *  Optional parameter, when set to false it will also try to BackTrackSolve() the board
 * @return
 *  Solvable
 */
bool Sudoku::BoardIsSolvable(Board board, bool scanSolveOnly) {
    if (board.IsValid()) {    
        bool solved = ScanSolve(&board); // Works on a copy of the original board

        if (!scanSolveOnly && !solved)
            solved = BackTrackSolve(&board); // Will make use of things already done by ScanSolve()

        return solved;
    }
    else
        return false;
}


//----------------------------------------------------------------------------
// Private methods.

/**
 * Tries to solve the board in the most natural way, by eliminating possibilities (and keeping track of which numbers are possible per position).
 * This is the 'easiest' method, and requires the least CPU cycles.
 *
 * @param board
 *  The board we are going to solve
 * @return
 *  Solvable
 */
bool Sudoku::ScanSolve(Board* board) {
    bool found = board->IsValid(); //Only enter the filling loop if the board is valid
    int numPossible;
    bool possibleSolutions[9][9][10]; // 10 to avoid confusion (element 0 is always false)
    for (int y = 0; y < 9; ++y) // Preparation; set all element 0's to false, and all the rest to true
        for (int x = 0; x < 9; ++x) {
            possibleSolutions[x][y][0] = false;
            for (int e = 1; e < 10; ++e)
                possibleSolutions[x][y][e] = true;
        }
    
    while (!board->IsFull() && found) { // While we still found possible moves
        found = false; // In found we keep if we have filled in an element
        for (int y = 0; y < 9; ++y)
            for (int x = 0; x < 9; ++x) {
                numPossible = 0; // We keep the number of possibilities
                for (int e = 1; e < 10; ++e) { // Looking for possible solutions per box
                    if (possibleSolutions[x][y][e]) { // If it hasn't already been marked as impossible (we're working incrementally)
                        possibleSolutions[x][y][e] = board->IsValidMove(x, y, e);
                        if (possibleSolutions[x][y][e]) // If it is still a valid move
                            ++numPossible;
                    }
                }
                if (numPossible == 1) {
                    for (int e = 1; e < 10; ++e)
                        if (possibleSolutions[x][y][e])
                            board->Set(x, y, e);
                    found = true;
                }
            }
    }
    
    return board->IsFull();
}

/**
 * Tries to solve the board by backtracking.
 * This is the 'hard' method, and requires a lot of CPU cycles.
 * The overhead is reduced if we always run the ScanSolve() algorithm first.
 *
 * @param board
 *  The board we are going to solve
 * @param startx, starty
 *  Startpositions (because we are working recursively)
 * @return
 *  Solvable
 */
bool Sudoku::BackTrackSolve(Board* board, int startx, int starty) {
    bool solved = board->IsFull(); // If we are at a leaf we have to return true, because the board is filled
    bool foundzero = false;
    int x = startx, y;
    
    for (y = starty; !solved && !foundzero && y < 9; ++y) { // Find the first element that is not filled in (we are working recursively)
        for (x = startx; !foundzero && x < 9; ++x)
            foundzero = (board->Get(x, y) == 0);
        startx = 0;
    }
    x--;
    y--;
    
    if (!solved && foundzero) { // Only if there is a 0 found we can still backtrack, else: end this recursion
        for (int e = 1; e < 10 && !solved; ++e)
            if (board->IsValidMove(x, y, e)) {
                board->Set(x, y, e);
                if (BackTrackSolve(board, x + 1, y))
                    solved = true;
                else
                    board->Remove(x, y);
            }
    }

    return solved;
}

/**
 * Generates a random solved board
 *
 * @param board
 *  Pointer to the memory location of the board
 */
void Sudoku::GenerateRandomSolution(Board* board) {
    int x, y, numberOfRandoms;
    int e;
    
    srand((unsigned int) time(NULL)); // Set random number seed
    do {
        board->Clear();
        numberOfRandoms = 25; // After lots of testing, 25 seemed like the right number of random items
        for (int i = 0; i < numberOfRandoms; ++i) {
            x = rand() % 9;
            y = rand() % 9;
            e = rand() % 9 + 1;
            if (board->IsValidMove(x, y, e))
                board->Set(x, y, e);
            else
                --i;
        }
    } while (!SolveBoard(board));
}

/**
 * Checks if the board can be solved by ScanSolve() when we remove a particular element from it
 *
 * @param board
 *  The board we are going to solve
 * @param x, y
 *  Coordinates of the element we want to remove
 *  
 * @return
 *  Solvable
 */
bool Sudoku::SolvableWithRemoval(Board* board, int x, int y) {
    bool output = false;
    
    if (board->Get(x, y) == 0)
        return false;
    
    int value = board->Get(x, y); // So we don't have to copy the board each time
    board->Remove(x, y);
    output = (BoardIsSolvable(*board, true)); // ScanSolve() only
    board->Set(x, y, value);
    return output;
}

/**
 * Calculates how many elements should be reinserted/removed from the board
 * according to the level. Helper function of GenerateBoard
 *
 * @param level
 *  The difficulty of the board
 * @return
 *  Number of moves to undo/remove (removal is negative)
 */
int Sudoku::LevelToNumMoves(int level) {
    int reinsert;
    
    switch (level) {
        case 1:
            reinsert = 4;
            break;
        case 2:
            reinsert = 3;
            break;
        case 3:
            reinsert = 2;
            break;
        case 4:
            reinsert = 1;
            break;
        case 5:
            reinsert = 0;
            break;
        case 6:
            reinsert = -2;
            break;
        case 7:
            reinsert = -5;
            break;
        default:
            reinsert = -5;
    }
    
    return reinsert;
}

/**
 * Checks if the board contains any blocks/rows/columns that are
 * completely filled in.
 * This is a helper function for GenerateBoard()
 *
 * @param board
 *  The board we want to check
 * @return
 *  Number of moves to undo/remove (removal is negative)
 */
bool Sudoku::BoardHasFilledParts(Board* board) {
    bool filledFound = false;
    
    // Rows
    for (int y = 0; y < 9 && !filledFound; ++y) {
        bool containsUnfilledElem = false;
        for (int x = 0; x < 9 && !containsUnfilledElem; ++x)
            containsUnfilledElem = (board->Get(x, y) == 0);
        filledFound = !containsUnfilledElem;
    }
    
    // Columns
    for (int x = 0; x < 9 && !filledFound; ++x) {
        bool containsUnfilledElem = false;
        for (int y = 0; y < 9 && !containsUnfilledElem; ++y)
            containsUnfilledElem = (board->Get(x, y) == 0);
        filledFound = !containsUnfilledElem;
    }
    
    //Blocks
    for (int startX = 0; !filledFound && startX < 9; startX += 3) // Iterate over the blocks, horizontally
        for (int startY = 0; !filledFound && startY < 9; startY += 3) { // Iterate over the blocks, vertically
            bool containsUnfilledElem = false;
            for (int i = startX; !containsUnfilledElem && (i % 3 != 0 || i == startX); ++i) // Iterate over the current block
                for (int j = startY; !containsUnfilledElem && (j % 3 != 0 || j == startY); ++j)
                    containsUnfilledElem = (board->Get(j, i) == 0);
            filledFound = !containsUnfilledElem;
        }
    
    return filledFound;
}
