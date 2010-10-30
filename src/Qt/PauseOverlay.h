// $Id: PauseOverlay.h 297 2008-05-30 20:16:20Z wimleers $


/**
 * PauseOverlay definition.
 *
 * @file PauseOverlay.h
 * @author Wim Leers
 */


#ifndef PAUSEOVERLAY_H
#define PAUSEOVERLAY_H


#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QCoreApplication>
#include <QPainter>
#include "Dimensions.h"
#include "SudokuElement.h"


class PauseOverlay : public QGraphicsItem {

    Q_DECLARE_TR_FUNCTIONS(PauseOverlay)

public:
    PauseOverlay(void) { }
    ~PauseOverlay(void) { }

    QRectF boundingRect(void) const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
};

#endif // PAUSEOVERLAY_H
