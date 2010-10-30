// $Id: SudokuGame.h 370 2008-06-08 21:50:31Z wimleers $


/**
 * SudokuGame class definition.
 *
 * SudokuGame is the Model in the MVC pattern. In this case, that means
 * it contains all game data and logic.
 * 
 * @file SudokuGame.h
 * @author Wim Leers
 */
 
 
#ifndef SUDOKUGAME_H
#define SUDOKUGAME_H


#include <QObject>
#include <QMetaType>
#include <QtDebug>
#include <QFile>
#include <QTimer>
#include <iostream>
#include "../Board.h"
#include "../Sudoku.h"
#include "../BoardGenerator.h"


class SudokuGame : public QObject {

    Q_OBJECT

public:
    SudokuGame(int difficulty = 1);
    SudokuGame(Board * board);
    ~SudokuGame(void);

    bool load(const QString fileName);
    bool save(const QString fileName);

    Board const * getBoard(void) const;
    Board const * getOriginalBoard(void) const;
    bool const * getChoices(int x, int y) const;
    void setChoices(int x, int y, bool const * choices);

    bool solve(void);

    bool isWorking(void) const;
    bool isValid(void) const;
    bool isFinished(void) const;

    // Game info calculations methods.
    bool validityCalculation(void) { return m_validityCalculationEnabled; }
    bool solvabilityCalculation(void) { return m_solvabilityCalculationEnabled; }
    bool statsCalculation(void) { return m_statsCalculationEnabled; }
    void enableValidityCalculation(bool enable) { m_validityCalculationEnabled = enable; }
    void enableSolvabilityCalculation(bool enable) { m_solvabilityCalculationEnabled = enable; }
    void enableStatsCalculation(bool enable) { m_statsCalculationEnabled = enable; }
    void performAllCalculations(bool firstTime = false);

    int getFinalChoice(int x, int y) const;

public slots:
    void start(void);
    void pause(bool pause);
    void reset(void);

    void enableChoice(int x, int y, int number);
    void disableChoice(int x, int y, int number);
    void setFinalChoice(int x, int y, int number);
    void unsetFinalChoice(int x, int y);

private slots:
    void updateDuration(void);
    void boardGenerated(int generationTime);

signals:
    void changed(int x, int y);
    void paused(bool paused);
    void finished(unsigned int duration);
    void validityChanged(bool valid, int lastFinalChoiceX, int lastFinalChoiceY);
    void solvabilityChanged(bool solvable, int lastFinalChoiceX, int lastFinalChoiceY);
    void statsChanged(int generated, int completed, int remaining);

    void ready(bool);

    void durationUpdated(unsigned int);

    void working(bool working);

private:
    // Game object construction helper methods.
    void initialize(int difficulty);
    void generateBoard(void);
    void storeOriginalBoard(void);
    void finishConstruction(void);

    // Calculation methods.
    void startWorking(void);
    void stopWorking(void);
    void calculateValidity(bool firstTime = false);
    void calculateSolvability(bool firstTime = false);
    void calculateStats(bool firstTime = false);

    Board * m_originalBoard;
    Board * m_board;
    bool m_choices[9][9][9];
    unsigned int m_duration;
    bool m_mustGenerateBoard;

    int m_lastFinalChoiceX;
    int m_lastFinalChoiceY;

    // States.
    bool m_ready;
    bool m_isLoadedGame;
    int m_difficulty;
    bool m_finished;
    bool m_working;
    bool m_valid; // This is a state, but there still is a special game info calculation setting for it, which can prevent unnecessary signals.

    // Game info calculation settings.
    bool m_validityCalculationEnabled;
    bool m_solvabilityCalculationEnabled;
    bool m_statsCalculationEnabled;

    // Timers.
    QTimer * m_durationTimer;

    // Sudoku board generation thread.
    BoardGenerator * m_thread;
};

#endif // SUDOKUGAME_H
