// $Id: SudokuScene.h 333 2008-06-07 13:22:48Z wimleers $


/**
 * SudokuScene class definition.
 *
 * @file SudokuScene.h
 * @author Wim Leers
 */


#ifndef SUDOKUSCENE_H
#define SUDOKUSCENE_H


#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QSettings>
#include "Dimensions.h"
#include "SudokuGame.h"
#include "SudokuElement.h"
#include "SudokuHUD.h"
#include "PauseOverlay.h"
#include "PauseOverlayEventFilter.h"


class SudokuScene : public QGraphicsScene {

    Q_OBJECT

public:
    SudokuScene(QAction * mainWindowPauseAction);
    virtual ~SudokuScene(void);

    void setGame(SudokuGame * game);

    void renderBoard(QPainter * painter, const QRectF & target = QRectF());

    // Qt-related methods.
    bool isWorking(void) const;
    void resizeScene(int width, int height);
  
public slots:
    void updateSudoku(void);
    void updateSudokuElement(int x, int y);

    // HUD-related slots.
    void validityEnabled(bool enabled);
    void solvabilityEnabled(bool enabled);
    void statsEnabled(bool enabled);

private slots:
    void pause(bool paused);
    void animationStep(void);
    void moveFocus(int fromX, int fromY, int toX, int toY);
    void loadHints(int x, int y);

    // Necessary when a new board is being generated in a separate thread.
    void resyncGeneratedElements(void);

signals:
    void moveFinished(void);
    void gameIsActive(bool active);

private:
    void setNewGameHelper(SudokuGame * game);
    void updateSudokuElementHelper(int x, int y);

    SudokuGame * m_game; // Not owned.
    SudokuGame * m_pendingNewGame; // Not owned.

    bool m_paused; // Used to check if the pause overlay is already added to the scene or not.    
    
    // Qt-related variables.
    QTimer * m_animationTimer;
    double m_currentScale;
    SudokuElement ** m_elements;
    QGraphicsRectItem * m_boxes;
    SudokuHUD * m_hud;
    PauseOverlay * m_pauseOverlay;
    PauseOverlayEventFilter * m_filter;
};

#endif // SUDOKUSCENE_H
