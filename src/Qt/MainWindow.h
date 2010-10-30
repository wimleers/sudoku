// $Id: MainWindow.h 333 2008-06-07 13:22:48Z wimleers $


/**
 * MainWindow class definition.
 *
 * @file MainWindow.h
 * @author Wim Leers
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QApplication>
#include <QSettings>
#include <QMainWindow>
#include <QMenuBar>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QImage>
#include <QtDebug>

#include "NewGameDialog.h"
#include "SudokuGame.h"
#include "SudokuView.h"
#include "SudokuScene.h"
#include "SudokuElement.h"
#include "SudokuHUD.h"


class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    MainWindow(QWidget * parent = NULL, Qt::WindowFlags flags = NULL);
    ~MainWindow(void) { }

    void deactivated(void);

    QWidget * m_central;
    QHBoxLayout * m_mainLayout;
    QVBoxLayout * m_gameInfoLayout;

private slots:
    void spawnNewGameDialog(void);

    void newGame(int difficulty);
    void newGame(SudokuGame * game = NULL, bool isLoaded = false);
    void loadGame(void);
    void saveGame(void);
    void solveGame(void);

    void importBoardFromCSV(void);
    void exportBoardToCSV(void);

    void fullScreen(bool enabled);
    void print(void);
    void screenshot(void);

    void gameFinished(unsigned int duration);

    void enableGameActions(bool enable);

private:
    void setupSudokuMenu(void);
    void setupSettingsMenu(void);
    void setupWindow(void);
    void setupOther(void);

    void pauseGameHelper(void);

    QString secondsToString(unsigned int numSeconds) const;
    
    // 'Sudoku' menu.
    QMenu * mSudoku;
    QAction * aNew;
    QAction * aOpen;
    QAction * aSave;
    QAction * aPause;
    QAction * aReset;
    QAction * aSolve;
    QAction * aImportFromCSV;
    QAction * aExportToCSV;
    QAction * aPrint;
    QAction * aExportToPNG;
    QAction * aFullScreen;
    QAction * aQuit;

    // 'Settings' menu.
    QMenu * mSettings;
    QAction * aValidity;
    QAction * aSolvability;
    QAction * aStats;

    // Sudoku game (model), scene and view (controller/view mix).
    SudokuGame * m_game;
    SudokuScene * m_scene;
    SudokuView * m_view;

    // New game dialog.
    NewGameDialog * m_newGameDialog;
};

#endif // MAINWINDOW_H
