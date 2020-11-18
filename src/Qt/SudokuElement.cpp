// $Id: SudokuElement.cpp 350M 2010-05-07 00:20:33Z (local) $


/**
 * Qt SudokuElement widget implementation.
 *
 * The choices' coordinates are (0, 0) at the left top and (2, 2) at the
 * bottom right.
 *
 * @file SudokuElement.cpp
 * @author Wim Leers
 */


#include "SudokuElement.h"


//----------------------------------------------------------------------------
// Constructor & destructor.

SudokuElement::SudokuElement(void) {
    m_x = -1;
    m_y = -1;
    m_finalChoice = -1;
    m_choices = new bool[9];
    m_focus = false;
    m_generated = false;

    for (int i = 0; i < 9; i++)
        m_choices[i] = false;

    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsFocusable);
}


//----------------------------------------------------------------------------
// Public methods.

/**
 * Implementation of the pure virtual boundingRect() method.
 */
QRectF SudokuElement::boundingRect(void) const {
    return QRectF(0, 0, Dimensions::elementSize, Dimensions::elementSize);
}

/**
 * Implementation of the pure virtual paint() method.
 */
void SudokuElement::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    Q_UNUSED(widget);

    // Use a gradient for the brush.
    QLinearGradient gradient = SudokuElement::getBackgroundGradient();
    gradient.setColorAt(0, (m_focus) ? QColor(170, 218, 238) : QColor(223, 243, 252));
    gradient.setColorAt(1, (m_focus) ? QColor(223, 243, 252) : Qt::white            );
    painter->setBrush(gradient);

    // Draw the box.
    painter->setPen(QPen(QColor(56, 165, 211), 1));
    painter->drawRect(0, 0, Dimensions::elementSize, Dimensions::elementSize);

    // If no final choice is made, then draw the 9 choices inside the box.
    // Otherwise, we draw the final choice at an item-filling size.
    if (m_finalChoice == -1) {
        // Increase the foint point size temporarily.
        QFont font = QFont(painter->font());
        font.setPixelSize(Dimensions::elementChoiceSize);
        painter->setFont(font);

        for (int number = 1; number <= 9; number++) {
            bool chosen = m_choices[number - 1];
            if (chosen || m_focus) { // We draw the number if it's either chosen or if the user is hovering over this item.

                // When chosen: dark blue pen, light blue-ish otherwise.
                painter->setPen(QPen((chosen) ? QColor(3, 73, 104) : QColor(87, 106, 114), 0));

                // When chosen: bold font.
                QFont font = QFont(painter->font());
                font.setBold(chosen);
                painter->setFont(font);

                // Draw!
                painter->drawText(
                    getBoundingRectForChoice(number),
                    Qt::AlignCenter | Qt::AlignVCenter,
                    QString::number(number)
                );
            }
        }
    }
    else {
        // Font.
        QFont font = QFont(painter->font());
        // Increase the font point size temporarily.
        font.setPixelSize(0.6 * Dimensions::elementSize);
        // Make the font bold if it's not a generated element.
        font.setBold(!m_generated);
        painter->setFont(font);

        // Pen.
        painter->setPen(QPen((m_generated) ? QColor(56, 165, 211) : Qt::black, 1));

        painter->drawText(
            getBoundingRectForFinalChoice(),
            Qt::AlignCenter | Qt::AlignVCenter,
            QString::number(m_finalChoice)
        );
    }
}

/**
 * Set all choices and schedule a repaint.
 *
 * @param choices
 *   An array of 9 bools, with false meaning that the choice is not set.
 */
void SudokuElement::setChoices(bool const * choices) {
    for (int i = 0; i < 9; i++)
        m_choices[i] = choices[i];

    update();
}

/**
 * Get the SudokuElement's generated state.
 */
bool SudokuElement::getGenerated(void) const {
    return m_generated;
}

/**
 * Set the SudokuElement's generated state.
 *
 * @param generated
 *   Whether this SudokuElement should be rendered as if it were generated.
 */
void SudokuElement::setGenerated(bool generated) {
    m_generated = generated;

    // TRICKY: we cannot just disable a generated element, because we could
    // then longer focus on it. And that's necessary for keyboard navigation.
    // So instead, we disregard all events except for keyboard navigation
    // events by checking if m_generated is true.
    //setEnabled(!generated);
}

/**
 * Enable a choice.
 *
 * @param number
 *   A number (1-9).
 */
void SudokuElement::enableChoice(int number) {
    if (number < 1 || number > 9)
        qFatal("SudokuElement::setChoice(): number was not in the valid range (1-9). number: %d.", number);

    m_choices[number - 1] = true;
}

/**
 * Disable a choice.
 *
 * @param number
 *   A number (1-9).
 */
void SudokuElement::disableChoice(int number) {
    if (number < 1 || number > 9)
        qFatal("SudokuElement::unsetChoice(): number was not in the valid range (1-9). number: %d.", number);

    m_choices[number - 1] = false;
}

/**
 * Set the final choice.
 *
 * @param number
 *   A number (1-9).
 */
void SudokuElement::setFinalChoice(int number) {
    if (number < 1 || number > 9)
        qFatal("SudokuElement::setFinalChoice(): number was not in the valid range (1-9). number: %d.", number);

    m_finalChoice = number;
}

/**
 * Unset the final choice.
 */
void SudokuElement::unsetFinalChoice(void) {
    m_finalChoice = -1;
}


//----------------------------------------------------------------------------
// Protected methods.

/**
 * Override of hoverEnterEvent().
 */
void SudokuElement::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
    Q_UNUSED(event);
    setFocus(Qt::OtherFocusReason);
}

/**
 * Override of hoverLeaveEvent().
 */
void SudokuElement::hoverLeaveEvent(QGraphicsSceneHoverEvent * event) {
    Q_UNUSED(event);
    clearFocus();
}

/**
 * Override of mousePressEvent().
 */
void SudokuElement::mousePressEvent(QGraphicsSceneMouseEvent * event) {
    if (m_generated) // This is a generated element, so ignore this event.
        return;

    if (m_finalChoice != -1)
        return;

    int number = getChoiceByMousePos(event->lastScenePos());
    if (number > 0) {
        // Send out signals to allow for responding events.
        if (!m_choices[number - 1])
            emit enableChoice(m_x, m_y, number);
        else
            emit disableChoice(m_x, m_y, number);
    }
}

/**
 * Override of mouseDoubleClickEvent().
 */
void SudokuElement::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event) {
    if (m_generated) // This is a generated element, so ignore this event.
        return;

    // If there's no final choice set yet, allow this double click to set it.
    if (m_finalChoice == -1) {
        int number = getChoiceByMousePos(event->lastScenePos());
        if (number > 0)
            emit setFinalChoice(m_x, m_y, number);
    }
    // Otherwise, if a final choice is set, allow this double click to unset
    // it.
    else if (QRectF(getBoundingRectForFinalChoice()).contains(mapFromScene(event->lastScenePos())))
        emit unsetFinalChoice(m_x, m_y);
}

/**
 * Override of contextMenuEvent().
 */
void SudokuElement::contextMenuEvent(QGraphicsSceneContextMenuEvent * event) {
    Q_UNUSED(event);

    // FIXME: This does not work in Mac OS X, due to a bug in Qt: this method
    // is never called!

    /*
    QMenu menu;
    QAction * hint = menu.addAction(tr("Get hints"));
    menu.show();
    */
}

/**
 * Override of focusInEvent().
 */
void SudokuElement::focusInEvent(QFocusEvent * event) {
    Q_UNUSED(event);

    m_focus = true;
    update();
}

/**
 * Override of focusOutEvent().
 */
void SudokuElement::focusOutEvent(QFocusEvent * event) {
    Q_UNUSED(event);

    m_focus = false;
    update();
}

/**
 * Override of keyPressEvent().
 */
void SudokuElement::keyPressEvent(QKeyEvent * event) {
    // Selection movement.
    switch (event->key()) {
        case Qt::Key_Left: // Left arrow.
            emit selectOtherSudokuElement(m_x, m_y, m_x - 1, m_y);
            break;
        case Qt::Key_Up: // Top arrow.
            emit selectOtherSudokuElement(m_x, m_y, m_x, m_y - 1);
            break;
        case Qt::Key_Right: // Right arrow.
            emit selectOtherSudokuElement(m_x, m_y, m_x + 1, m_y);
            break;
        case Qt::Key_Down: // Bottom arrow.
            emit selectOtherSudokuElement(m_x, m_y, m_x, m_y + 1);
            break;
    }



    // This is a generated element, so only allow keyboard navigation.
    if (m_generated)
        return;



    // Final choices and choices.
    if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_9) {
        int number = event->key() - Qt::Key_0;

        #ifdef Q_OS_MACX
            #define CHOICE_MODIFIER Qt::AltModifier
        #else
            #define CHOICE_MODIFIER Qt::ControlModifier
        #endif

        if (event->modifiers() != CHOICE_MODIFIER) // Final choice.
            if (number != m_finalChoice)
                emit setFinalChoice(m_x, m_y, number);
            else
                emit unsetFinalChoice(m_x, m_y);
        else // Choice.
            if (!m_choices[number - 1])
                emit enableChoice(m_x, m_y, number);
            else
                emit disableChoice(m_x, m_y, number);
    }

    // Unset a final choice.
    if (event->key() == Qt::Key_Backspace)
        emit unsetFinalChoice(m_x, m_y);

    if (event->key() == Qt::Key_H)
        emit loadHints(m_x, m_y);
}


//----------------------------------------------------------------------------
// Private methods.

/**
 * Get the number by passing in a mouse position (from the scene).
 *
 * @param scenePos
 *   The mouse position on the scene.
 * @return
 *   An integer between 0 and 9:
 *   - 0 if the position does not map to a number
 *   - 1-9 if it does map
 */
int SudokuElement::getChoiceByMousePos(const QPointF & scenePos) const {
    QPointF itemPos = mapFromScene(scenePos);

    for (int number = 1; number <= 9; number++)
        // NOTE: if it turns out to be hard for the user to click the number,
        // we can always upscale the bounding rectangle for the number,
        // allowing him to click just beside it.
        if (QRectF(getBoundingRectForChoice(number)).contains(itemPos))
            return number;

    return 0;
}

/**
 * Get the bounding rectangle for a number, and determine the number by its
 * position in the SudokuElement ((x, y) coordinates, where x, y = {0, 1, 2}).
 *
 * @param x
 *   x coordinate of a number in the SudokuElement
 * @param y
 *   y coordinate of a number in the SudokuElement
 * @return
 *   A bounding rectangle
 */
QRect SudokuElement::getBoundingRectForChoiceByCoords(int x, int y) const {
    if (x < 0 || x > 2 || y < 0 || y > 2)
        qFatal("SudokuElement::getBoundingRectForChoiceByCoords(): x or y was not in the valid range (0-2). x: %d, y: %d.", x, y);

    int offset = (Dimensions::elementSize - (3 * Dimensions::elementChoiceSize)) / 4;
    int nextNumberOffset = offset + Dimensions::elementChoiceSize;

    return QRect(
        offset + x * nextNumberOffset,
        offset + y * nextNumberOffset,
        Dimensions::elementChoiceSize,
        Dimensions::elementChoiceSize
    );
}

/**
 * Get the bounding rectangle for a number.
 *
 * @param number
 *   A number (1-9).
 * @return
 *   A bounding rectangle.
 */
QRect SudokuElement::getBoundingRectForChoice(int number) const {
    if (number < 1 || number > 9)
        qFatal("SudokuElement::getboundingRectForChoice(): number was not in the valid range (1-9). number: %d.", number);

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++)
            if (--number == 0)
                return getBoundingRectForChoiceByCoords(x, y);

    // This should never be reached!
    qFatal("The number %d could not be found!", number);
    return QRect();
}

/**
 * Get the bounding rectangle for the final choice.
 *
 * @return
 *   A bounding rectangle.
 */
QRect SudokuElement::getBoundingRectForFinalChoice(void) const {
    return QRect(ceil(0.1 * Dimensions::elementSize), ceil(0.1 * Dimensions::elementSize), ceil(0.8 * Dimensions::elementSize), ceil(0.8 * Dimensions::elementSize));
}

/**
 * Get the background gradient, without the colors set. This allows us to
 * generate the gradient only *once*!
 *
 * @return
 *   A gradient object for which only the colors still have to be set.
 */
QLinearGradient SudokuElement::getBackgroundGradient(void) {
    static bool generated = false;
    static QLinearGradient gradient;

    if (!generated) {
        double start = (double) Dimensions::elementSize / 6;
        double end = (double) Dimensions::elementSize * 5 / 6;
        gradient = QLinearGradient(QPointF(start, start), QPointF(end, end));

        generated = true;
    }

    return gradient;
}
