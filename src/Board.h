// $Id: Board.h 297 2008-05-30 20:16:20Z wimleers $


/**
 * Board class definition.
 *
 * @file Board.h
 * @author Bram Bonne
 */


#ifndef BOARD_H
#define BOARD_H

#include <QMetaType>
#include "FileIO.h"
#include "InternalException.h"
#include <qdatastream.h>
#ifdef Q_OS_LINUX
#include <cstdlib>
#endif
#include <string>
using namespace std;

class Board {
public:
    Board(void);
    Board(const Board & other);

    int Get(int x, int y) const { return m_board[x][y]; }
    void Set(int x, int y, int e) { m_board[x][y] = e; }
    void Remove(int x, int y) { m_board[x][y] = 0; }
    void Clear(void);
    bool IsValidMove(int x, int y, int e) const;
    bool IsValid(void) const;
    bool IsFull(void) const;
    int NumFilledIn(void) const;
    bool* GetPossibleMoves(int x, int y) const;

    bool Import(const string & fname);
    bool Export(const string & fname) const;

    Board & operator=(const Board & other);
    const int* operator[](unsigned int i) const;
    int* operator[](unsigned int i);

    friend QDataStream& operator<<(QDataStream& out, const Board& board);
    friend QDataStream& operator>>(QDataStream& in, Board& board);

private:
    bool CheckHorizontal(int x, int y, int e) const;
    bool CheckVertical(int x, int y, int e) const;
    bool CheckBlock(int x, int y, int e) const;
    bool IsValidHorizontal() const;
    bool IsValidVertical() const;
    bool IsValidBlocks() const;

    int m_board[9][9];
};

Q_DECLARE_METATYPE(Board)

#endif //BOARD_H
