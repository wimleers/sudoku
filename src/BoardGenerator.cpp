// $Id: BoardGenerator.cpp 297M 2010-05-07 00:21:20Z (local) $


/**
 * BoardGenerator implementation.
 *
 * @file BoardGenerator.cpp
 * @author Wim Leers
 */


#include "BoardGenerator.h"


//----------------------------------------------------------------------------
// Public methods.

/**
 * Generates a new board. Notifies the calling thread via the boardGenerated()
 * signal when finished.
 *
 * @see BoardGenerator::run()
 */
void BoardGenerator::QueueBoardGeneration(Board * board, int level) {
    m_board = board;
    m_level = level;

    start(QThread::NormalPriority);
}

void BoardGenerator::run() {
    Board * board = m_board;
    int level = m_level;

    // Start measuring time.
    QTime m_generationTime;
    m_generationTime.start();

    Sudoku::GenerateBoard(board, level);
    
    // Make sure the generation took at least 0.5 seconds, to make sure the
    // animation doesn't just flash.
    unsigned long actualGenerationTime = (unsigned long) m_generationTime.elapsed();
    if (actualGenerationTime < 500)
        msleep((unsigned long) 500 - actualGenerationTime);

    emit boardGenerated(actualGenerationTime);
}
