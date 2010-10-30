// $Id: Board.cpp 345 2008-06-07 19:28:19Z wimleers $

#include "Board.h"


//----------------------------------------------------------------------------
// Public methods.

/**
 * Creates a new instance of Board
 */
Board::Board(void) {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            m_board[i][j] = 0;
}

/**
 * Creates a new instance of Board
 *
 * @param other
 *   Board that needs to be copied
 */
Board::Board(const Board & other) {
    operator=(other);
}

/**
 * Clears all values of the board.
 */
void Board::Clear(void) {
    for (int y = 0; y < 9; ++y)
        for (int x = 0; x < 9; ++x)
            Remove(x, y);
}

/**
 * Checks if a move is valid.
 *
 * @param x
 *   x-pos
 * @param y
 *   x-pos
 * @param e
 *   number we want to add
 * @return
 *  Valid or invalid (already there)
 * @warning
 *  Method assumes that the board is valid before the move
 */
bool Board::IsValidMove(int x, int y, int e) const {
    if (CheckHorizontal(x, y, e) && CheckVertical(x, y, e) && CheckBlock(x, y, e)) // Lazy evaluation guarantees efficient handling
        return true;
    else
        return false;
}

/**
 * Checks if the board itself is valid.
 *
 * @return
 *  Valid or invalid
 */
bool Board::IsValid(void) const {
    return (IsValidHorizontal() && IsValidVertical() && IsValidBlocks()); // Lazy evaluation guarantees efficient handling
}

/**
 * Checks if the board is full.
 *
 * @return
 *  Valid or invalid
 */
bool Board::IsFull(void) const {
    bool full = true;
    
    for (int i = 0; full && i < 9; ++i)
        for (int j = 0; full && j < 9; ++j)
            full = (m_board[i][j] != 0);
    
    return full;
}

/**
 * Counts the number of filled in elements.
 *
 * @return
 *  Quantity
 */
int Board::NumFilledIn(void) const {
    int num = 0;
    
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            num += (m_board[i][j] != 0) ? 1 : 0;
    
    return num;
}

/**
 * Gives the possible moves for a given position.
 *
 * @return
 *  The possible moves, in a boolean array
 */
bool* Board::GetPossibleMoves(int x, int y) const {
    bool* output = new bool[10];
    
    output[0] = 0;
    for (int i = 1; i < 10; i++)
        output[i] = IsValidMove(x, y, i);
    
    return output;
}

/**
 * Loads a saved board from a file
 *
 * @param fname
 *  Name of the file to load
 * @return
 *  True if saved board is found and successfully loaded
 */
bool Board::Import(const string & fname) {
    FileIO file;
    
    file.SetFileName(fname);
    file.Open();
    FILEIO_TABLE table = file.GetTable();

    if (table.size() == 9) {
        FILEIO_TABLE::iterator tableIt = table.begin();
        FILEIO_ROW::iterator rowIt = tableIt->begin();
        
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) { // Read a row from the table
                m_board[j][i] = atoi(rowIt->c_str());
                rowIt++;
            }
            tableIt++;
            rowIt = tableIt->begin();
        }
        return true;
    }
    else
        return false;
}

/**
 * Saves the board to a file
 *
 * @param fname
 *  Name of file to save to
 * @return
 *  True if board is successfully saved
 */
bool Board::Export(const string & fname) const {
    FileIO file;
    
    FILEIO_TABLE table;
    for (int i = 0; i < 9; ++i) {
        FILEIO_ROW row;
        for (int j = 0; j < 9; ++j) {
            char s[2]; // Because it needs to be added as a char*
            s[0] = m_board[j][i] + '0';
            s[1] = '\0';
            row.push_back(s); // Fill a row
        }
        table.push_back(row); // Add the row to the table w're going to save
    }
    
    file.SetFileName(fname); // Prepare FileIO object for saving.
    file.SetTable(table);
    
    return file.Save(); // Save returns true if all is successful.
}


/**
 * Copies the other board to this board.
 *
 * @param other
 *  The board which we are going to copy
 * @return
 *  The copied board
 */
Board & Board::operator=(const Board & other) {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            m_board[i][j] = other.Get(i, j);
    
    return *this;
}

/**
 * Returns a row of the board, is constant.
 *
 * @param i
 *  Position in the array
 * @return
 *  The demanded row
 */
const int* Board::operator[](unsigned int i) const {
    return m_board[i];
}

/**
 * Returns a row of the board, is not constant.
 *
 * @param i
 *  Position in the array
 * @return
 *  The demanded row
 */
int* Board::operator[](unsigned int i) {
    return m_board[i];
}


//----------------------------------------------------------------------------
// Private methods.

/**
 * Checks a vertical row for the given element.
 *
 * @param x
 *   x-pos
 * @param y
 *   x-pos
 * @param e
 *   Element we want to add
 * @return
 *  Valid or invalid (already there)
 */
bool Board::CheckHorizontal(int x, int y, int e) const {
    bool valid = (m_board[x][y] == 0); // Position can't already be filled in
    
    for (int i = 0; valid && i < 9; ++i)
        valid = (m_board[i][y] != e);
    
    return valid;
}

/**
 * Checks a horizontal row for the given element.
 *
 * @param x
 *   x-pos
 * @param y
 *   x-pos
 * @param e
 *   Element we want to add
 * @return
 *  Valid or invalid (already there)
 */
bool Board::CheckVertical(int x, int y, int e) const {
    bool valid = (m_board[x][y] == 0); // Position can't already be filled in
    
    for (int i = 0; i < 9 && valid; ++i)
        valid = (m_board[x][i] != e);

    return valid;
}

/**
 * Checks a block for the given element.
 *
 * @param x
 *   x-pos
 * @param y
 *   x-pos
 * @param e
 *   Element we want to add
 * @return
 *  Valid or invalid (already there)
 */
bool Board::CheckBlock(int x, int y, int e) const {
    bool valid = (m_board[x][y] == 0);
    int startx, starty; // Position of the first element in a block
    
    startx = x - (x % 3); // Highest product of 3
    starty = y - (y % 3);
    
    for (int i = starty; valid && (i % 3 != 0 || i == starty); ++i) // While not in the next block, and not at end of the board
        for (int j = startx; valid && (j % 3 != 0 || j == startx); ++j)
            valid = (m_board[j][i] != e);
    
    return valid;
}

/**
 * Checks if the board is valid in horizontal direction (helper function for IsValid() ).
 *
 * @return
 *  Valid or invalid (two or more of the same element)
 */
bool Board::IsValidHorizontal(void) const {
    bool elementOccured[10];
    bool valid = true;
    
    for (int i = 0; valid && i < 9; ++i) { // Iterate over the different rows of the board
        for (int j = 0; j < 10; ++j) // Initialize the boolean array
            elementOccured[j] = false;
        for (int j = 0; valid && j < 9; ++j) {// Iterate over the current row
            int curElem = m_board[j][i];
            valid = !elementOccured[curElem]; // Board is still valid if element hasn't occured yet
            if (curElem != 0)
                elementOccured[curElem] = true;
    	}
    }
    
    return valid;
}

/**
 * Checks if the board is valid in vertical direction (helper function for IsValid() ).
 *
 * @return
 *  Valid or invalid (two or more of the same element)
 */
bool Board::IsValidVertical(void) const {
    bool elementOccured[10];
    bool valid = true;
    
    for (int i = 0; valid && i < 9; ++i) { // Iterate over the different columns of the board
        for (int j = 0; j < 10; j++) // Initialize the boolean array
            elementOccured[j] = false;
        for (int j = 0; valid && j < 9; ++j) {// Iterate over the current column
            int curElem = m_board[i][j];
            valid = !elementOccured[curElem]; // Board is still valid if element hasn't occured yet
            if (curElem != 0)
                elementOccured[curElem] = true;
        }
    }
    
    return valid;
}

/**
 * Checks if the blocks in the board are valid (helper function for IsValid() ).
 *
 * @return
 *  Valid or invalid (two or more of the same element)
 */
bool Board::IsValidBlocks(void) const {
    bool elementOccured[10];
    bool valid = true;
    
    for (int startX = 0; valid && startX < 9; startX += 3) // Iterate over the blocks, horizontally
        for (int startY = 0; valid && startY < 9; startY += 3) { // Iterate over the blocks, vertically
            for (int j = 0; j < 10; ++j) // Initialize the boolean array
                elementOccured[j] = false;
            for (int i = startX; valid && (i % 3 != 0 || i == startX); ++i) // Iterate over the current block
                for (int j = startY; valid && (j % 3 != 0 || j == startY); ++j) {
                    int curElem = m_board[j][i];
                    valid = !elementOccured[curElem]; // Board is still valid if element hasn't occured yet
                    if (curElem != 0)
                        elementOccured[curElem] = true;
                }
        }
    
    return valid;
}

/**
 * Writes the board to a QDataStream.
 *
 * @param out
 *  The output stream
 * @param board
 *  The board we want to write out
 * @return
 *  The output stream (to be able to do << again)
 */
QDataStream& operator<<(QDataStream& out, const Board& board) {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
                out << board[j][i];
    
    return out;
}

/**
 * Reads the board from a QDataStream
 *
 * @param in
 *  The input stream
 * @param board
 *  The board we want to write out
 * @return
 *  The input stream (to be able to do >> again)
 */
QDataStream& operator>>(QDataStream& in, Board& board) {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
                in >> board[j][i];
    
    return in;
}
