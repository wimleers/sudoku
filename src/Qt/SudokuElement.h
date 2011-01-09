// $Id: SudokuElement.h 350 2008-06-07 20:52:53Z wimleers $


/**
 * Qt SudokuElement widget definition.
 *
 * This class is derived from QGraphicsItem, because it will be used as an
 * item in a QGraphicsScene. However, it's derived first from QObject, because
 * we want to be able to emit signals.
 *
 * @file SudokuElement.h
 * @author Wim Leers
 */


#ifndef SUDOKUELEMENT_H
#define SUDOKUELEMENT_H


#include <QObject>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QRectF>
#include <QPointF>
#include <QLinearGradient>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QKeyEvent>
#include <math.h>
#include "Dimensions.h"


class SudokuElement : public QObject, public QGraphicsItem {

    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    SudokuElement();

    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    void setChoices(bool const * choices);
    bool getGenerated(void) const;
    void setGenerated(bool generated);
    int getX(void) const { return m_x; }
    int getY(void) const { return m_y; }
    void setX(int x) { m_x = x; }
    void setY(int y) { m_y = y; }

signals:
    void enableChoice(int x, int y, int number);
    void disableChoice(int x, int y, int number);
    void setFinalChoice(int x, int y, int number);
    void unsetFinalChoice(int x, int y);
    void loadHints(int x, int y);
    void selectOtherSudokuElement(int fromX, int fromY, int toX, int toY);

public slots:
    void enableChoice(int number);
    void disableChoice(int number);
    void setFinalChoice(int number);
    void unsetFinalChoice(void);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);

    void focusInEvent(QFocusEvent * event);
    void focusOutEvent(QFocusEvent * event);

    void keyPressEvent(QKeyEvent * event);

private:
    int getChoiceByMousePos(const QPointF & pos) const;
    QRect getBoundingRectForChoiceByCoords(int x, int y) const;
    QRect getBoundingRectForChoice(int number) const;
    QRect getBoundingRectForFinalChoice(void) const;

    static QLinearGradient getBackgroundGradient(void);

    // Values.
    int m_x, m_y;
    bool * m_choices;
    int m_finalChoice;

    // States.
    bool m_focus;
    bool m_generated;
};

#endif // SUDOKUELEMENT_H
