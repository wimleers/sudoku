// $Id: SudokuHUD.h 350 2008-06-07 20:52:53Z wimleers $


/**
 * Qt SudokuHUD widget definition.
 *
 * The heads-up display for a Sudoku game.
 * 
 * @file SudokuHUD.h
 * @author Wim Leers
 */


#ifndef SUDOKUHUD_H
#define SUDOKUHUD_H


#include <QObject>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QRectF>
#include <QPointF>
#include <QLinearGradient>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <math.h>
#include "Dimensions.h"


class SudokuHUD : public QObject, public QGraphicsItem {

    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    SudokuHUD(void);
    ~SudokuHUD(void);

    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    void validityEnabled(bool enabled) { m_validityEnabled = enabled; }
    void solvabilityEnabled(bool enabled) { m_solvabilityEnabled = enabled; }
    void statsEnabled(bool enabled) { m_statsEnabled = enabled; }

    // Public constants.
    static const int width = 150;

signals:
    void pauseGame(void);

public slots:
    void duration(unsigned int duration);
    void validity(bool validity);
    void solvability(bool solvability);
    void stats(int generated, int completed, int remaining);

    void gameLoaded(bool gameLoaded) { m_gameLoaded = gameLoaded; }

    void calculating(bool calculating);

private slots:
    void repaintSpinner(void);

private:
    QRect getBoundingRectForTimer(void) const;
    QRect getBoundingRectForValidity(void) const;
    QRect getBoundingRectForSolvability(void) const;
    QRect getBoundingRectForStats(void) const;
    QRect getBoundingRectForSpinner(void) const;
    QRect getBoundingRectForSpinnerText(void) const;

    QString secondsToString(unsigned int numSeconds) const;

    static QLinearGradient getBackgroundGradient(void);

    QTimer * m_animTimer;

    // The 8 variables.
    bool m_gameLoaded;
    unsigned int m_duration;
    bool m_validity;
    bool m_solvability;
    int m_generated;
    int m_completed;
    int m_remaining;
    bool m_calculating;

    // Settings.
    bool m_validityEnabled;
    bool m_solvabilityEnabled;
    bool m_statsEnabled;
};

#endif // SUDOKUHUD_H
