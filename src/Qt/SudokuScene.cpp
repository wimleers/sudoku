// $Id: SudokuScene.cpp 358 2008-06-08 13:13:41Z wimleers $


/**
 * Qt SudokuScene implementation.
 *
 * @file SudokuScene.cpp
 * @author Wim Leers
 */


#include "SudokuScene.h"


//----------------------------------------------------------------------------
// Constructor & destructor.

SudokuScene::SudokuScene(QAction * mainWindowPauseAction) {
    m_game = NULL;
    m_paused = false;
    m_currentScale = 1.0;
    m_pauseOverlay = new PauseOverlay();
    m_pauseOverlay->setZValue(1);

    // Create the event filter that will be used when the pause overlay is
    // active.
    m_filter = new PauseOverlayEventFilter(this, mainWindowPauseAction);

    setSceneRect(0, 0, Dimensions::sceneWidth, Dimensions::sceneHeight);

    // Allocate all 81 SudokuElement widgets and all 9 QGraphicsRectItem
    // widgets that will be used as boxes.
    m_elements = new SudokuElement * [9];
    m_boxes = new QGraphicsRectItem [9];
    for (int i = 0; i < 9; i++)
        m_elements[i] = new SudokuElement [9];

    // Now add them to the scene.
    int xOffset = 0, yOffset = 0;
    for (int i = 0; i < 9; i++) {
        xOffset += (i == 3 || i == 6) ? Dimensions::margin : 0;
        yOffset = 0;
        for (int j = 0; j < 9; j++) {
            yOffset += (j == 3 || j == 6) ? Dimensions::margin : 0;
            SudokuElement * e = (&m_elements[i][j]);
            e->setPos(xOffset + Dimensions::margin + Dimensions::elementSize * i, yOffset + Dimensions::margin + Dimensions::elementSize * j);
            e->setX(i);
            e->setY(j);
            addItem(e);
            
            if ((i + 1) % 3 == 0 && (j + 1) % 3 == 0) {
                // Accentuate the boxes with thicker lines.
                static int box_id = -1;
                box_id++;
                m_boxes[box_id].setRect(QRectF(xOffset + Dimensions::margin + Dimensions::elementSize * (i - 2), yOffset + Dimensions::margin + Dimensions::elementSize * (j - 2), Dimensions::elementSize * 3, Dimensions::elementSize * 3));
                m_boxes[box_id].setPen(QPen(QColor(36, 156, 206), 2));
                addItem(&m_boxes[box_id]);
            }
        }
    }

    // Allocate the HUD and add it to the scene.
    m_hud = new SudokuHUD();
    m_hud->setPos(Dimensions::boardSize + Dimensions::margin, Dimensions::margin);
    addItem(m_hud);

    // Set up the animation timer.
    // TODO: animations!
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, SIGNAL(timeout()), SLOT(animationStep()));
}

SudokuScene::~SudokuScene(void) {
    for (int i = 0; i < 9; i++)
        delete m_elements[i];
    delete m_elements;

    m_animationTimer->stop();
    delete m_animationTimer;
    delete m_pauseOverlay;
    delete m_filter;
}


//----------------------------------------------------------------------------
// Public methods.

/**
 * Set a new game, but if an animation is still in progress, then wait until
 * it has finished.
 *
 * @param game
 *   A SudokuGame object.
 */
void SudokuScene::setGame(SudokuGame * game) {
    // If an animation is still executing, then we must wait until the
    // animation has finished before we can set the new game. If we don't do
    // this, animationStep() might try to act on the already deleted object,
    // causing a crash.
    if (this->isWorking() || (m_game && m_game->isWorking()))
        m_pendingNewGame = game;
    else {
        pause(false); // Remove the pause overlay immediately because we're setting a new game!
        setNewGameHelper(game);
    }
}

/**
 * When a board is being generated in a separate thread, it's necessary to
 * resync the generated elements to show them in the scene.
 */
void SudokuScene::resyncGeneratedElements(void) {
    Board const * originalBoard = m_game->getOriginalBoard();
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            m_elements[i][j].setGenerated(originalBoard->Get(i, j) > 0);
            m_elements[i][j].setEnabled(true);
        }

    m_game->performAllCalculations(true);
    updateSudoku();
}

/**
 * Wrapper to make it easy to render only the board, and without a possible
 * pause overlay. Typically used for printing the board or exporting to a
 * image.
 *
 * @see QGraphicsScene::render()
 */
void SudokuScene::renderBoard(QPainter * painter, const QRectF & target) {
    bool wasPaused = m_paused;
    if (wasPaused)
        pause(false);

    render(painter, target, QRect(0, 0, Dimensions::boardSize, Dimensions::boardSize));

    if (wasPaused)
        pause(true);    
}

/**
 * Check if the scene is working (i.e. if animations are in progress).
 */
bool SudokuScene::isWorking(void) const {
    return m_animationTimer->isActive();
}

/**
 * Resize the scene, including all SudokuElements.
 *
 * @param width
 *   The new width of the scene (in pixels).
 * @param height
 *   The new height of the scene (in pixels).
 */
void SudokuScene::resizeScene(int width, int height) {
    setSceneRect(0, 0, width, height);

    // If the new aspect ratio (width to height) is greater than the initial
    // one, we must scale using width, otherwise using height, to maintain
    // the aspect ratio of the scene (and as a result to keep all items of the
    // scene visible).
    double scale;
    if ((double) width / height > Dimensions::sceneRatio())
        scale = (double) height / Dimensions::sceneHeight;
    else
        scale = (double) width / Dimensions::sceneWidth;

    // Store in CPU registry because this will be used to scale *every* item
    // in the scene!    
    register double scaledScale = scale / m_currentScale;

    QGraphicsItem * item;
    foreach (item, this->items()) {
        item->setPos(item->pos() * scaledScale);
        item->setScale(scale);
    }

    m_currentScale = scale;
}


//----------------------------------------------------------------------------
// Public slots.

void SudokuScene::updateSudoku(void) {
    for (int x = 0; x < 9; x++)
        for (int y = 0; y < 9; y++)
            updateSudokuElementHelper(x, y);

    update(QRect(Dimensions::margin, Dimensions::margin, Dimensions::boardSize, Dimensions::boardSize));
}

void SudokuScene::updateSudokuElement(int x, int y) {
    updateSudokuElementHelper(x, y);

    // TODO: add a tighter bounding rectangle to improve speed.
    update(QRect(Dimensions::margin, Dimensions::margin, Dimensions::boardSize, Dimensions::boardSize));
}

void SudokuScene::validityEnabled(bool enabled) {
    m_game->enableValidityCalculation(enabled);
    m_hud->validityEnabled(enabled);

    QSettings settings;
    settings.setValue("hud/validity", enabled);    

    if (enabled)
        m_game->performAllCalculations(true);

    update(QRect(Dimensions::boardSize + Dimensions::margin, Dimensions::margin, Dimensions::HUDWidth, Dimensions::HUDHeight));
}

void SudokuScene::solvabilityEnabled(bool enabled) {
    m_game->enableSolvabilityCalculation(enabled);
    m_hud->solvabilityEnabled(enabled);

    QSettings settings;
    settings.setValue("hud/solvability", enabled);

    if (enabled)
        m_game->performAllCalculations(true);

    update(QRect(Dimensions::boardSize + Dimensions::margin, Dimensions::margin, Dimensions::HUDWidth, Dimensions::HUDHeight));
}

void SudokuScene::statsEnabled(bool enabled) {
    m_game->enableStatsCalculation(enabled);
    m_hud->statsEnabled(enabled);

    QSettings settings;
    settings.setValue("hud/stats", enabled);

    if (enabled)
        m_game->performAllCalculations(true);

    update(QRect(Dimensions::boardSize + Dimensions::margin, Dimensions::margin, Dimensions::HUDWidth, Dimensions::HUDHeight));
}


//----------------------------------------------------------------------------
// Private slots.

/**
 * Handle all animations in the scene.
 */
void SudokuScene::animationStep(void) {
    if (m_pendingNewGame != NULL)
        setNewGameHelper(m_pendingNewGame);
    else {
        // TODO: animations.

        emit moveFinished();
    }
}

/**
 * Show a PauseOverlay when the game is paused.
 *
 * @param paused
 *   true when the game is paused, false otherwise.
 */
void SudokuScene::pause(bool paused) {
    if (!m_paused && paused) {
        m_pauseOverlay->setScale(m_currentScale);
        addItem(m_pauseOverlay);
        installEventFilter(m_filter);
        update(QRectF(0, 0, Dimensions::boardSize, Dimensions::boardSize));
    }
    else if (m_paused && !paused) {
        removeItem(m_pauseOverlay);
        double scaleBack = 1.0 / m_currentScale;
        m_pauseOverlay->setScale(scaleBack);
        removeEventFilter(m_filter);
        update(QRectF(0, 0, Dimensions::boardSize, Dimensions::boardSize));
    }
    m_paused = paused;
}

/**
 * Move the focus from one element to another. Used for keyboard navigation.
 * 
 * @param fromX
 *   The x-coordinate of the element that has focus.
 * @param fromY
 *   The y-coordinate of the element that has focus.
 * @param toX
 *   The x-coordinate of the element that the focus will move to.
 * @param toY
 *   The y-coordinate of the element that the focus will move to.
 */
void SudokuScene::moveFocus(int fromX, int fromY, int toX, int toY) {
    Q_UNUSED(fromX);
    Q_UNUSED(fromY);

    // TODO: animation?

    toX = (toX < 0) ? 8 : toX % 9;
    toY = (toY < 0) ? 8 : toY % 9;
    setFocusItem(&m_elements[toX][toY], Qt::PopupFocusReason);
}

/**
 * Load the hints for an element.
 * 
 * @param x
 *   The x-coordinate of the element for which the hints will be loaded.
 * @param y
 *   The y-coordinate of the element for which the hints will be loaded.
 */
void SudokuScene::loadHints(int x, int y) {
    bool * hints = m_game->getBoard()->GetPossibleMoves(x, y);
    
    // We received an array of 10 bools, with the first array element being
    // meaningless. Convert to an array of 9 bools.
    bool * preparedHints = new bool[9];
    for (int i = 0; i < 9; i++)
        preparedHints[i] = hints[i + 1];

    m_game->setChoices(x, y, preparedHints);
    
    delete hints;
    delete preparedHints;
}


//----------------------------------------------------------------------------
// Private methods.

/**
 * Set a new game (assume that no animation is in progress), make the
 * necessary connections, etc.
 * 
 * @param game
 *   A SudokuGame object.
 */
void SudokuScene::setNewGameHelper(SudokuGame * game) {
     m_animationTimer->stop();

     // If an old game exists (i.e. if this isn't the first game of this
     // session), disconnect all signals and delete it from memory.
     if (m_game != NULL) {
        // The current game ended, make the HUD stop displaying info.
         m_hud->gameLoaded(false);

         for (int i = 0; i < 9; i++) {
             for (int j = 0; j < 9; j++) {
                 disconnect(&m_elements[i][j], SIGNAL(enableChoice(int, int, int)),   m_game, SLOT(enableChoice(int, int, int)));
                 disconnect(&m_elements[i][j], SIGNAL(disableChoice(int, int, int)),  m_game, SLOT(disableChoice(int, int, int)));
                 disconnect(&m_elements[i][j], SIGNAL(setFinalChoice(int, int, int)), m_game, SLOT(setFinalChoice(int, int, int)));
                 disconnect(&m_elements[i][j], SIGNAL(unsetFinalChoice(int, int)),    m_game, SLOT(unsetFinalChoice(int, int)));
             }
         }
         disconnect(m_game, SIGNAL(changed(int, int)), this, SLOT(updateSudokuElement(int, int)));
         disconnect(m_game, SIGNAL(paused(bool)),      this, SLOT(pause(bool)));
         disconnect(m_game, SIGNAL(ready(bool)),       this, SLOT(resyncGeneratedElements()));

         // HUD connections.
         disconnect(m_game, SIGNAL(validityChanged(bool, int, int)),    m_hud, SLOT(validity(bool)));
         disconnect(m_game, SIGNAL(solvabilityChanged(bool, int, int)), m_hud, SLOT(solvability(bool)));
         disconnect(m_game, SIGNAL(statsChanged(int, int, int)),        m_hud, SLOT(stats(int, int, int)));
         disconnect(m_game, SIGNAL(durationUpdated(unsigned int)),      m_hud, SLOT(duration(unsigned int)));
         disconnect(m_game, SIGNAL(working(bool)),                      m_hud, SLOT(calculating(bool)));
         disconnect(m_game, SIGNAL(ready(bool)),                        m_hud, SLOT(gameLoaded(bool)));

         delete m_game;
     }

    m_game = game;
    m_pendingNewGame = NULL; // If this was a pending game, it's now active!

    // Connect signals.
    Board const * originalBoard = m_game->getOriginalBoard();
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (originalBoard != NULL)
                m_elements[i][j].setGenerated(originalBoard->Get(i, j) > 0);
            connect(&m_elements[i][j], SIGNAL(enableChoice(int, int, int)),   m_game, SLOT(enableChoice(int, int, int)));
            connect(&m_elements[i][j], SIGNAL(disableChoice(int, int, int)),  m_game, SLOT(disableChoice(int, int, int)));
            connect(&m_elements[i][j], SIGNAL(setFinalChoice(int, int, int)), m_game, SLOT(setFinalChoice(int, int, int)));
            connect(&m_elements[i][j], SIGNAL(unsetFinalChoice(int, int)),    m_game, SLOT(unsetFinalChoice(int, int)));

            connect(&m_elements[i][j], SIGNAL(loadHints(int, int)),                          this, SLOT(loadHints(int, int)));
            connect(&m_elements[i][j], SIGNAL(selectOtherSudokuElement(int, int, int, int)), this, SLOT(moveFocus(int, int, int, int)));
        }
    }
    connect(m_game, SIGNAL(changed(int, int)), this, SLOT(updateSudokuElement(int, int)));
    connect(m_game, SIGNAL(paused(bool)),      this, SLOT(pause(bool)));
    connect(m_game, SIGNAL(ready(bool)),       this, SLOT(resyncGeneratedElements()));

    // HUD connections.
    connect(m_game, SIGNAL(validityChanged(bool, int, int)),    m_hud, SLOT(validity(bool)));
    connect(m_game, SIGNAL(solvabilityChanged(bool, int, int)), m_hud, SLOT(solvability(bool)));
    connect(m_game, SIGNAL(statsChanged(int, int, int)),        m_hud, SLOT(stats(int, int, int)));
    connect(m_game, SIGNAL(durationUpdated(unsigned int)),      m_hud, SLOT(duration(unsigned int)));
    connect(m_game, SIGNAL(working(bool)),                      m_hud, SLOT(calculating(bool)));
    connect(m_game, SIGNAL(ready(bool)),                        m_hud, SLOT(gameLoaded(bool)));
    
    // Give focus to the SudokuElement in the middle!
    setFocusItem(&m_elements[4][4], Qt::PopupFocusReason);

    updateSudoku();

    // Start the game:
    // - when this is a saved game or an import: starts the game immediately.
    // - when this is a new game for which a board has to be generated: start
    //   generating the board and after that finished, actually start the game.
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            m_elements[i][j].setEnabled(false);
    m_game->start();

    emit gameIsActive(true);
}


void SudokuScene::updateSudokuElementHelper(int x, int y) {
    // Final choice.
    int value = m_game->getFinalChoice(x, y);
    if (value == -1)
        m_elements[x][y].unsetFinalChoice();
    else
        m_elements[x][y].setFinalChoice(value);

    // Choices.
    m_elements[x][y].setChoices(m_game->getChoices(x, y));
}
