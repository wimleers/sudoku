// $Id: SudokuGame.cpp 370 2008-06-08 21:50:31Z wimleers $


/**
 * Qt SudokuGame implementation.
 *
 * @file SudokuGame.cpp
 * @author Wim Leers
 */


#include "SudokuGame.h"


//----------------------------------------------------------------------------
// Constructors & destructor.

SudokuGame::SudokuGame(int difficulty) {
    m_mustGenerateBoard = true;
    initialize(difficulty);
    m_board = new Board();
    m_originalBoard = NULL;
    m_duration = 0;
    m_durationTimer = NULL;
    m_thread = NULL;

    // Actual board generation is done in a separate method (generateBoard()),
    // to allow for precise timing of this CPU-intensive process.
}

SudokuGame::SudokuGame(Board * board) {
    m_mustGenerateBoard = false;
    initialize(-1);
    m_board = board;
    storeOriginalBoard();
}

SudokuGame::~SudokuGame(void) {
    delete m_board;
    delete m_originalBoard;

    if (m_durationTimer != NULL) {
        m_durationTimer->stop();
        disconnect(m_durationTimer, SIGNAL(timeout()), this, SLOT(updateDuration()));
        delete m_durationTimer;
    }
}


//----------------------------------------------------------------------------
// Public methods.

/**
 * Load a SudokuGame (unserialize from file).
 * 
 * @author
 *  Bram Bonne
 * @param fileName
 *  A filename.
 * @return
 *  true in case of a succesful load, false otherwise.
 */
bool SudokuGame::load(const QString fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        // This is a game that's being loaded, so we definitely don't need to
        // generate a new board. We do need to mark this game as a loaded game
        // because otherwise the duration would be reset!
        m_mustGenerateBoard = false;
        m_isLoadedGame      = true;

        QDataStream stream(&file);
        stream >> *m_board;
        // m_originalBoard is initialized to NULL, so we have to stream it
        // into a temporary variable first.
        Board *ob = new Board();
        stream >> *ob;
        m_originalBoard = ob;

        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                for (int k = 0; k < 9; k++)
                    stream >> m_choices[k][j][i];

        stream >> m_lastFinalChoiceX >> m_lastFinalChoiceY;
        stream >> m_difficulty >> m_finished >> m_working >> m_valid;
        stream >> m_duration;
        stream >> m_validityCalculationEnabled >> m_solvabilityCalculationEnabled >> m_statsCalculationEnabled;

        return true;
    }
    else
        return false;
}

/**
 * Save a SudokuGame object (serialize to file).
 * 
 * @author
 *  Bram Bonne
 * @param fileName
 *  A filename.
 * @return
 *  true in case of a succesful save, false otherwise.
 */
bool SudokuGame::save(const QString fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream stream(&file);
        stream << *m_board << *m_originalBoard;

        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                for (int k = 0; k < 9; k++)
                    stream << m_choices[k][j][i];

        stream << m_lastFinalChoiceX << m_lastFinalChoiceY;
        stream << m_difficulty << m_finished << m_working << m_valid;
        stream << m_duration;
        stream << m_validityCalculationEnabled << m_solvabilityCalculationEnabled << m_statsCalculationEnabled;

        return true;
    }
    else
        return false;
}

/**
 * Retrieve the current board for read-only access.
 *
 * @return
 *   The current board.
 */
Board const * SudokuGame::getBoard(void) const {
    return m_board;
}

/**
 * Retrieve the original board for read-only access.
 *
 * @return
 *   The original board.
 */
Board const * SudokuGame::getOriginalBoard(void) const {
    return m_originalBoard;
}
 
/**
 * Retrieve the choices for an element for read-only access.
 * 
 * @param x
 *   A valid x-coordinate (0-8).
 * @param y
 *   A valid y-coordinate (0-8).
 * @return
 *   The choices for the element at the given x- and y-coordinates.
 */   
bool const * SudokuGame::getChoices(int x, int y) const {
    return m_choices[x][y];
}

/**
 * Set all choices for an element.
 * 
 * @param x
 *   A valid x-coordinate (0-8).
 * @param y
 *   A valid y-coordinate (0-8).

 * @param choices
 *   An array of 9 bools, with false meaning that the choice is not set.
 */
void SudokuGame::setChoices(int x, int y, bool const * choices) {
    for (int i = 0; i < 9; i++)
        m_choices[x][y][i] = choices[i];

    emit changed(x, y);
}


/**
 * Solve the game.
 *
 * @return
 *   true if the game could be solved, false otherwise.
 */   
bool SudokuGame::solve(void) {
    bool status;

    startWorking();
    status = Sudoku::SolveBoard(m_board);
    stopWorking();

    // TODO: animations?
    for (int x = 0; x < 9; x++)
        for (int y = 0; y < 9; y++)
            emit changed(x, y);

    // Recalculate only the stats, since validity and solvability will remain
    // positive by definition.
    calculateStats();
    
    // Stop the timer and reset the duration to 0, to indicate that the user
    // didn't solve the Sudoku.
    m_durationTimer->stop();
    m_duration = 0;
    emit durationUpdated(m_duration);

    return status;
}

/**
 * Check if the game is working (i.e. if calculations are being made).
 * 
 * @return
 *   true if the game is working, false otherwise.
 */
bool SudokuGame::isWorking(void) const {
    return m_working;
}

/**
 * Check if board is valid.
 * 
 * @return
 *   true if the board is valid, false otherwise.
 */
bool SudokuGame::isValid(void) const {
    return m_valid;
}

bool SudokuGame::isFinished(void) const {
    return m_finished;
}

/**
 * Get the final choice for a certain Sudoku element.
 * 
 * @param x
 *   A valid x-coordinate (0-8).
 * @param y
 *   A valid y-coordinate (0-8).
 * @return
 *   The final choice (1-9) for the given Sudoku element, -1 if not set.
 */
int SudokuGame::getFinalChoice(int x, int y) const {
    int finalChoice = (int) m_board->Get(x, y);

    return (finalChoice >= 1 && finalChoice <= 9) ? finalChoice : -1;
}

/**
 * Trigger all state checking functions, causing the corresponding signals to
 * be sent.
 * 
 * @param firstTime
 *   If true, a signal will always be sent.
 */
void SudokuGame::performAllCalculations(bool firstTime) {
    emit durationUpdated(m_duration);

    calculateValidity(firstTime);

    if (m_solvabilityCalculationEnabled)
        calculateSolvability(firstTime);

    if (m_statsCalculationEnabled)
        calculateStats(firstTime);
}


//----------------------------------------------------------------------------
// Public slots.

/**
 * Pause/unpause the game.
 * 
 * @param pause
 *   true if the game should be paused, false if it should be paused.
 */
void SudokuGame::pause(bool pause) {
    if (!m_ready)
        return;

    if (pause) {
        m_durationTimer->stop();
    }
    else
        m_durationTimer->start();

    emit paused(pause);
}

/**
 * Reset the game.
 */
void SudokuGame::reset(void) {
    startWorking();
    delete m_board;
    m_board = new Board(*m_originalBoard);
    performAllCalculations(true);
    for (int x = 0; x < 9; x++)
        for (int y = 0; y < 9; y++) {
            for (int i = 0; i < 9; i++)
                m_choices[x][y][i] = false;
            emit changed(x, y);
        }
    stopWorking();
}


/**
 * Enable a choice.
 *
 * @param x
 *   A valid x-coordinate (0-8).
 * @param y
 *   A valid y-coordinate (0-8).
 * @param number
 *   A number (1-9).
 */
void SudokuGame::enableChoice(int x, int y, int number) {
    qDebug() << "SudokuGame::enableChoice() signal received, x =" << x << ", y =" << y << ", number =" << number;

    m_choices[x][y][number - 1] = true;

    emit changed(x, y);
}

/**
 * Disable a choice.
 *
 * @param x
 *   A valid x-coordinate (0-8).
 * @param y
 *   A valid y-coordinate (0-8).
 * @param number
 *   A number (1-9).
 */
void SudokuGame::disableChoice(int x, int y, int number) {
    qDebug() << "SudokuGame::disableChoice() signal received, x =" << x << ", y =" << y << ", number =" << number;

    m_choices[x][y][number - 1] = false;

    emit changed(x, y);
}

/**
 * Set the final choice.
 *
 * @param x
 *   A valid x-coordinate (0-8).
 * @param y
 *   A valid y-coordinate (0-8).
 * @param number
 *   A number (1-9).
 */
void SudokuGame::setFinalChoice(int x, int y, int number) {
    m_board->Set(x, y, (char) number);
    m_lastFinalChoiceX = x;
    m_lastFinalChoiceY = y;
    performAllCalculations();

    #ifndef QT_NO_DEBUG_OUTPUT 
        qDebug() << "SudokuGame::setFinalChoice()" << "Set final choice (" << number << ") for (" << x << "," << y << ").";
    #endif

    emit changed(x, y);

    if (m_board->IsFull() && m_board->IsValid()) {
        m_finished = true;
        m_durationTimer->stop();
        emit finished(m_duration);
    }
}

/**
 * Unset the final choice.
 * 
 * @param x
 *   A valid x-coordinate (0-8).
 * @param y
 *   A valid y-coordinate (0-8).
 */
void SudokuGame::unsetFinalChoice(int x, int y) {
    m_board->Remove(x, y);
    performAllCalculations();

    #ifndef QT_NO_DEBUG_OUTPUT 
        qDebug() << "SudokuGame::unsetFinalChoice()" << "Unset final choice for (" << x << "," << y << ").";
    #endif

    emit changed(x, y);
}


//----------------------------------------------------------------------------
// Private slots.

/**
 * Slot that's connected to a timer that fires every second. This allows us
 * to store the time and send a notification to the main window.
 */
void SudokuGame::updateDuration(void) {
    m_duration++;
    emit durationUpdated(m_duration);
}

/**
 * This slot is called when a new board has been generated. It finishes the
 * construction of a SudokuGame object.
 */
void SudokuGame::boardGenerated(int generationTime) {
    qDebug() << "Finished generating new board in worker thread in" << generationTime << "ms.";

    stopWorking();
    
    disconnect(m_thread, SIGNAL(boardGenerated(int)), this, SLOT(boardGenerated(int)));

    storeOriginalBoard();
    finishConstruction();
}


//----------------------------------------------------------------------------
// Private methods.

/**
 * Convenience method that contains code shared by all constructors.
 */
void SudokuGame::initialize(int difficulty) {
    m_ready        = false;
    m_isLoadedGame = false;
    m_difficulty   = difficulty;
    m_finished     = false;
    m_working      = false;
    m_valid        = true;

    m_validityCalculationEnabled    = true;
    m_solvabilityCalculationEnabled = true;
    m_statsCalculationEnabled       = true;


    // Set all all 729 choice booleans (9 per SudokuElement, of which there
    // are 81) to false.
    for (int x = 0; x < 9; x++)
        for (int y = 0; y < 9; y++)
            for (int z = 0; z < 9; z++)
                m_choices[x][y][z] = false;
}

void SudokuGame::generateBoard(void) {
    qDebug() << "Start generating new board...";
    startWorking();
    qRegisterMetaType<Board>("Board");
    m_thread = new BoardGenerator(this);
    connect(m_thread, SIGNAL(boardGenerated(int)), this, SLOT(boardGenerated(int)));
    m_thread->QueueBoardGeneration(m_board, m_difficulty);
}

void SudokuGame::storeOriginalBoard(void) {
    // Store the original board separately. This allows us to reset the Sudoku
    // and mark some elements as generated elements, even after interrupting
    // the application.
    m_originalBoard = new Board();
    *m_originalBoard = *m_board;
}

void SudokuGame::start(void) {
    if (!m_mustGenerateBoard)
        finishConstruction();
    else
        generateBoard();
}

void SudokuGame::finishConstruction(void) {
    m_ready = true;
    emit ready(true);
    qDebug() << "Game is ready!";

    performAllCalculations(true);

    // Start the duration timer.
    if (!m_isLoadedGame)
        m_duration = 0;
    m_durationTimer = new QTimer(this);
    connect(m_durationTimer, SIGNAL(timeout()), this, SLOT(updateDuration()));
    m_durationTimer->start(1000);
}

/**
 * Convenience method for managing the m_working property: sets it to true.
 */
inline void SudokuGame::startWorking(void) {
    m_working = true;
    emit working(true);
}

/**
 * Convenience method for managing the m_working property: sets it to false.
 */
inline void SudokuGame::stopWorking(void) {
    m_working = false;
    emit working(false);
}

/**
 * Calculates the validity of the current board and sends a signal when the
 * validity changes.
 * This method is different form calculateSolvability() and calculateStats()
 * in that it uses (and updates!) the m_valid state instead of using a static
 * variable to detect changes. The m_valid state is used for the game's flow
 * as well so this method is also called for internal use. That's why the
 * check for m_validityCalculationEnabled is *inside* the method, unlike the
 * other calculation methods.
 * 
 * @param firstTime
 *   If true, a signal will always be sent.
 */
void SudokuGame::calculateValidity(bool firstTime) {
    if (!m_ready)
        return;

    bool previously = m_valid;

    startWorking();
    m_valid = m_board->IsValid();
    stopWorking();

    if (m_validityCalculationEnabled) {
        if (firstTime)
            emit validityChanged(m_valid, m_lastFinalChoiceX, m_lastFinalChoiceY);
        else if (previously && !m_valid)
            emit validityChanged(false, m_lastFinalChoiceX, m_lastFinalChoiceY);
        else if (!previously && m_valid)
            emit validityChanged(true, m_lastFinalChoiceX, m_lastFinalChoiceY);
    }
}

/**
 * Calculates the solvability of the current board and sends a signal when
 * the solvability changes.
 * 
 * @param firstTime
 *   If true, a signal will always be sent.
 */
void SudokuGame::calculateSolvability(bool firstTime) {
    if (!m_ready)
        return;

    static bool previously = true;

    startWorking();
    bool solvable = Sudoku::BoardIsSolvable(*m_board, false);
    stopWorking();

    if (firstTime)
        emit solvabilityChanged(solvable, m_lastFinalChoiceX, m_lastFinalChoiceY);
    else if (previously && !solvable)
        emit solvabilityChanged(false, m_lastFinalChoiceX, m_lastFinalChoiceY);
    else if (!previously && solvable)
        emit solvabilityChanged(true, m_lastFinalChoiceX, m_lastFinalChoiceY);

    previously = solvable;
}

/**
 * Calculates the statistics of the current board and sends a signal when
 * the statistics change.
 * 
 * @param firstTime
 *   If true, a signal will always be sent.
 */
void SudokuGame::calculateStats(bool firstTime) {
    if (!m_ready)
        return;

    static int prevGenerated = 0, prevCompleted = 0, prevRemaining = 0;
    int generated, completed, remaining;

    startWorking();
    // Calculate the generated and completed amounts.
    generated = m_originalBoard->NumFilledIn();
    completed = m_board->NumFilledIn();

    // The completed amount counted too many: we have to substract the number
    // of generated elements.
    completed -= generated;
    // The remaining amount is 81 minus the other two amounts.
    remaining = 81 - generated - completed;
    stopWorking();

    if (firstTime)
        emit statsChanged(generated, completed, remaining);
    else if (prevCompleted != completed || prevGenerated != generated)
        emit statsChanged(generated, completed, remaining);

    prevGenerated = generated;
    prevCompleted = completed;
    prevRemaining = remaining;
}
