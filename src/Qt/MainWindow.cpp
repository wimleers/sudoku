// $Id: MainWindow.cpp 373 2008-06-09 00:44:58Z wimleers $


/**
 * MainWindow class implementation.
 *
 * @file MainWindow.cpp
 * @author Wim Leers
 */


#include "MainWindow.h"


//----------------------------------------------------------------------------
// Constructor.

MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags) : QMainWindow(parent, flags) {
    aPause = NULL; // to prevent crashes when deactivating the app when aPause hasn't been loaded yet.

    setupSudokuMenu();
    setupSettingsMenu();

    m_game = NULL;
    m_scene = new SudokuScene(aPause);
    enableGameActions(false);
    m_view = new SudokuView(m_scene);
    m_view->setEnabled(false);
    setCentralWidget(m_view);

    #ifdef Q_OS_MACX
        extern void qt_mac_set_dock_menu(QMenu *);
        qt_mac_set_dock_menu(mSudoku);
        qDebug() << "[MACOSX] " << "Set the 'Game' menu as a Dock menu.";
    #endif

    setupWindow();
    setupOther();

	setAttribute(Qt::WA_DeleteOnClose, true);

    // The MainWindow has to be displayed first, or no Sheet-style window will
    // be used on Mac OS X!
    show();

    // Start a new game by default.
    spawnNewGameDialog();
}


//----------------------------------------------------------------------------
// Public methods.

/**
 * The application was deactivated: pause the game!
 */
void MainWindow::deactivated(void) {
    pauseGameHelper();
}


//----------------------------------------------------------------------------
// Private slots.

/**
 * Spawn a NewGameDialog. This is a modal dialog, so the main window will
 * block automatically.
 */
void MainWindow::spawnNewGameDialog(void) {
    pauseGameHelper();

    m_newGameDialog = new NewGameDialog(this);
    m_newGameDialog->show();
    connect(m_newGameDialog, SIGNAL(newGame(int)), this, SLOT(newGame(int)));
}

/**
 * Create a new game with the given difficulty.
 */
void MainWindow::newGame(int difficulty) {
    if (m_newGameDialog != NULL) {
        disconnect(m_newGameDialog, SIGNAL(newGame(int)), this, SLOT(newGame(int)));
        delete m_newGameDialog;
    }
    newGame(new SudokuGame(difficulty), false);
}

/**
 * Create or load a game, update the scene and all signals.
 */
void MainWindow::newGame(SudokuGame * game, bool isLoaded) {
    m_view->setEnabled(true);
    
    // Disconnect old signals if this isn't the first game.
    if (m_game != NULL) {
        // Make sure the current game isn't paused when the new game is loaded!
        aPause->setChecked(false);

        disconnect(m_game, SIGNAL(finished(unsigned int)),             this, SLOT(gameFinished(unsigned int)));
        disconnect(aPause, SIGNAL(toggled(bool)),                      m_game, SLOT(pause(bool)));
        disconnect(aReset, SIGNAL(triggered()),                        m_game, SLOT(reset()));
    }

    // If no pointer to a SudokuGame object was passed, create one.
    if (game == NULL)
        game = new SudokuGame(1);

    m_game = game;
    m_scene->setGame(m_game);
    
    // Connect all signals and trigger an update of all states, causing the
    // corresponding signals to be sent.
    connect(m_game, SIGNAL(finished(unsigned int)),             this, SLOT(gameFinished(unsigned int)));
    connect(aPause, SIGNAL(toggled(bool)),                      m_game, SLOT(pause(bool)));
    connect(aReset, SIGNAL(triggered()),                        m_game, SLOT(reset()));

    // If this game is being loaded, then restore its settings.
    if (isLoaded) {
        aValidity->setChecked(m_game->validityCalculation());
        aSolvability->setChecked(m_game->solvabilityCalculation());
        aStats->setChecked(m_game->statsCalculation());
    }

    // Pass through the HUD settings.
    m_scene->validityEnabled(aValidity->isChecked());
    m_scene->solvabilityEnabled(aSolvability->isChecked());
    m_scene->statsEnabled(aStats->isChecked());
}

/**
 * Load a game.
 */
void MainWindow::loadGame(void) {
    pauseGameHelper();

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open a Sudoku Game"),
        "~/Documents",
        tr("Sudoku saved game (*.sud)")
    );

    if (!fileName.isEmpty()) {
        SudokuGame * game = new SudokuGame();
        if (game->load(fileName))
            newGame(game, true);
        else
            QMessageBox::warning(this, tr("Failed opening saved game"), tr("The saved game could not be opened. Perhaps the file is corrupt?"));
    }
}

/**
 * Save a game.
 */
void MainWindow::saveGame(void) {
    pauseGameHelper();

    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save a Sudoku Game"),
        "~/Documents/untitled.sud",
        tr("Sudoku saved game (*.sud)")
    );

    if (!fileName.isEmpty() && !m_game->save(fileName))
        QMessageBox::warning(this, tr("Failed saving the game"), tr("The game could not be saved. Perhaps you don't have sufficient permissions?"));
}

/**
 * Solve a game.
 */
void MainWindow::solveGame(void) {
    m_game->reset();
    m_game->solve();
}

/**
 * Import a board from a CSV file.
 */
void MainWindow::importBoardFromCSV(void) {
    pauseGameHelper();

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Import from CSV"),
        "~/Documents",
        tr("CSV file (*.csv)")
    );

    if (!fileName.isEmpty()) {
        Board * board = new Board();
        if (board->Import(fileName.toStdString())) {
            SudokuGame * game = new SudokuGame(board);
            newGame(game);
        }
        else
            QMessageBox::warning(this, tr("Invalid board"), tr("The board could not be imported, the format is invalid."));
    }
}

/**
 * Export a board to a CSV file.
 */
void MainWindow::exportBoardToCSV(void) {
    pauseGameHelper();

    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export to CSV"),
        "~/Documents/untitled.csv",
        tr("CSV file (*.csv)")
    );

    if (!fileName.isEmpty())
        m_game->getBoard()->Export(fileName.toStdString());
}

/**
 * Show the game in full screen, or go back to the normal view.
 */
void MainWindow::fullScreen(bool enabled) {
    if (enabled)
        showFullScreen();
    else
        showNormal();
}

/**
 * Print the current Sudoku puzzle.
 */
void MainWindow::print(void) {
    pauseGameHelper();

    // Set up the printer.
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);

    // Configure a dialog, execute it, and on accept, print the Sudoku!
    QPrintDialog * dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Sudoku Puzzle"));
    if (dialog->exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        m_scene->renderBoard(&painter);
    }
    delete dialog;
}

/**
 * Make a screenshot of the current Sudoku puzzle.
 */
void MainWindow::screenshot(void) {
    pauseGameHelper();

    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Screenshot"),
        "~/Desktop/Sudoku.png",
        tr("PNG image file (*.png)")
    );
    
    if (!fileName.isEmpty()) {
        QImage image(Dimensions::boardSize, Dimensions::boardSize, QImage::Format_ARGB32); 

        QPainter painter(&image);
        m_scene->renderBoard(&painter);

        image.save(fileName, "PNG");
    }
}

/**
 * The user finished the game!
 */
void MainWindow::gameFinished(unsigned int duration) {
    m_view->setEnabled(false);
    enableGameActions(false);
    QMessageBox::information(this, tr("Congratulations!"), tr("Congratulations, you finished this Sudoku in %1!").arg(secondsToString(duration)));
    // TODO: animations?
}

void MainWindow::enableGameActions(bool enable) {
    aSave->setEnabled(enable);

    aPause->setEnabled(enable);
    aReset->setEnabled(enable);
    aSolve->setEnabled(enable);

    aExportToCSV->setEnabled(enable);
    aPrint->setEnabled(enable);
    aExportToPNG->setEnabled(enable);

    aValidity->setEnabled(enable);
    aSolvability->setEnabled(enable);
    aStats->setEnabled(enable);
}


//----------------------------------------------------------------------------
// Private methods.

/**
 * Set up the 'Sudoku' menu.
 */
void MainWindow::setupSudokuMenu(void) {
    #ifdef Q_OS_MACX
        mSudoku = menuBar()->addMenu(tr("Game"));
        qDebug() << "[MACOSX] " << "Renamed the 'Sudoku' menu to 'Game' to prevent duplicate menu names.";
    #else
        mSudoku = menuBar()->addMenu(tr("Sudoku"));
    #endif

    aNew = mSudoku->addAction(tr("New..."));
    aNew->setShortcut(tr("Ctrl+N"));
    connect(aNew, SIGNAL(triggered()), this, SLOT(spawnNewGameDialog()));

    mSudoku->addSeparator();

    aOpen = mSudoku->addAction(tr("Open saved game..."));
    aOpen->setShortcut(tr("Ctrl+O"));
    connect(aOpen, SIGNAL(triggered()), this, SLOT(loadGame()));
    aSave = mSudoku->addAction(tr("Save..."));
    aSave->setShortcut(tr("Ctrl+S"));
    connect(aSave, SIGNAL(triggered()), this, SLOT(saveGame()));

    mSudoku->addSeparator();

    aPause = mSudoku->addAction(tr("Pause game"));
    aPause->setShortcut(tr("P"));
    aPause->setCheckable(true);
    aPause->setChecked(false);
    aReset = mSudoku->addAction(tr("Reset game"));
    aReset->setShortcut(tr("R"));
    aSolve = mSudoku->addAction(tr("Solve game"));
    aSolve->setShortcut(tr("S"));
    connect(aSolve, SIGNAL(triggered()), this, SLOT(solveGame()));

    mSudoku->addSeparator();

    aImportFromCSV = mSudoku->addAction(tr("Import from CSV..."));
    aImportFromCSV->setShortcut(tr("Ctrl+I"));
    connect(aImportFromCSV, SIGNAL(triggered()), this, SLOT(importBoardFromCSV()));
    aExportToCSV = mSudoku->addAction(tr("Export to CSV..."));
    aExportToCSV->setShortcut(tr("Ctrl+E"));
    connect(aExportToCSV, SIGNAL(triggered()), this, SLOT(exportBoardToCSV()));
    aExportToPNG = mSudoku->addAction(tr("Export to PNG..."));
    aExportToPNG->setShortcut(tr("Ctrl+R"));
    connect(aExportToPNG, SIGNAL(triggered()), this, SLOT(screenshot()));
    aPrint = mSudoku->addAction(tr("Print..."));
    aPrint->setShortcut(QKeySequence::Print);
    connect(aPrint, SIGNAL(triggered()), this, SLOT(print()));
    
    mSudoku->addSeparator();

    aFullScreen = mSudoku->addAction(tr("Full screen"));
    aFullScreen->setShortcut(tr("Ctrl+F"));
    aFullScreen->setCheckable(true);
    aFullScreen->setChecked(false);
    connect(aFullScreen, SIGNAL(toggled(bool)), this, SLOT(fullScreen(bool)));

    #ifndef Q_OS_MACX
        mSudoku->addSeparator();

        aQuit = mSudoku->addAction(tr("Quit"));
        aQuit->setShortcut(tr("Ctrl+Q"));
        connect(aQuit, SIGNAL(triggered()), this, SLOT(close()));
    #endif
}

/**
 * Set up the 'Settings' menu.
 */
void MainWindow::setupSettingsMenu(void) {
    QSettings settings;

    mSettings = menuBar()->addMenu(tr("Settings"));

    aValidity = mSettings->addAction(tr("Show board validity"));
    aValidity->setShortcut(tr("Alt+V"));
    aValidity->setCheckable(true);
    aValidity->setChecked(settings.value("hud/validity", true).toBool());

    aSolvability = mSettings->addAction(tr("Show board solvability"));
    aSolvability->setShortcut(tr("Alt+S"));
    aSolvability->setCheckable(true);
    aSolvability->setChecked(settings.value("hud/solvability", false).toBool());

    mSettings->addSeparator();

    aStats = mSettings->addAction(tr("Show stats"));
    aStats->setShortcut(tr("Alt+Shift+S"));
    aStats->setCheckable(true);
    aStats->setChecked(settings.value("hud/stats", true).toBool());
}

/**
 * Set up the window.
 */
void MainWindow::setupWindow(void) {
    // Window title and icon.
    setWindowTitle(tr("Sudoku"));
    setWindowIcon(QIcon(":/resources/images/icon.png"));


    // Set the Window size and center it.
    int height = Dimensions::sceneHeight;
    int width = Dimensions::sceneWidth;
    QDesktopWidget * d = QApplication::desktop();
    int x = (d->width() - width) / 2;
    int y = (d->height() - height) / 2;
    setGeometry(x, y, width, height);
    setMinimumSize(width, height);

    qDebug() << "[general]" << "Detected desktop resolution:" << d->width() << "x" << d->height();
    qDebug() << "[general]" << "Window size:" << width << "x" << height;
    qDebug() << "[general]" << "Window position: (" << x << "," << y << ")";

    #ifdef Q_OS_MACX
        // Use the unified title and toolbar look on Mac OS X 10.4 and later.
        setUnifiedTitleAndToolBarOnMac(true);
        qDebug() << "[MACOSX]" << "Use the unified title and toolbar.";
    #endif
}

/**
 * Set up other stuff.
 */
void MainWindow::setupOther(void) {
    // SudokuView repaints relatively slow, prevent performance issues.
    setAnimated(false);

    connect(m_scene, SIGNAL(gameIsActive(bool)), this, SLOT(enableGameActions(bool)));
    // Connect HUD settings signals.
    connect(aValidity, SIGNAL(toggled(bool)), m_scene, SLOT(validityEnabled(bool)));
    connect(aSolvability, SIGNAL(toggled(bool)), m_scene, SLOT(solvabilityEnabled(bool)));
    connect(aStats, SIGNAL(toggled(bool)), m_scene, SLOT(statsEnabled(bool)));
}

void MainWindow::pauseGameHelper(void) {
    if (aPause != NULL && m_game != NULL && !m_game->isFinished())
        aPause->setChecked(true);
}

/**
 * Convert a number of seconds into a human readable string, e.g. if you pass
 * in 73 as the number of seconds, you will receive "01:13".
 * 
 * @param numSeconds
 *   A number of seconds.
 * @return
 *   A human readable string.
 */
QString MainWindow::secondsToString(unsigned int numSeconds) const {
    unsigned int minutes = numSeconds / 60;
    unsigned int seconds = numSeconds % 60;

    QString secondsString = QString::number(seconds);
    if (secondsString.length() == 1)
        secondsString = "0" + secondsString;

    return QString::number(minutes) + ":" + secondsString;
}
