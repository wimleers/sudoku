// $Id: PauseOverlay.cpp 297 2008-05-30 20:16:20Z wimleers $


/**
 * PauseOverlay implementation.
 * 
 * @file PauseOverlay.cpp
 * @author Wim Leers
 */


#include "PauseOverlay.h"


//----------------------------------------------------------------------------
// Public methods.

/**
 * Implementation of the pure virtual boundingRect() method.
 */
QRectF PauseOverlay::boundingRect(void) const {
    return QRectF(0, 0, Dimensions::boardSize, Dimensions::boardSize);
}

/**
 * Implementation of the pure virtual paint() method.
 */
void PauseOverlay::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    Q_UNUSED(widget);
    
    // Only draw the part that is exposed to the user. From:
    // http://thesmithfam.org/blog/2007/02/03/qt-improving-qgraphicsview-performance/.
    painter->setClipRect(option->exposedRect);

    // Draw the transparency box.
    painter->setPen(QPen(Qt::black, 10));
    painter->setBrush(QColor(1, 1, 1, 200));
    painter->drawRect(0, 0, Dimensions::boardSize, Dimensions::boardSize);
    
    // Draw the overlaybox.
    painter->setPen(QPen(Qt::black, 5));
    painter->setBrush(Qt::white);
    int offset = Dimensions::margin + 2 * Dimensions::elementSize;
    painter->drawRect(offset, offset, Dimensions::pauseOverlaySize, Dimensions::pauseOverlaySize);

    // Draw the text.
    QFont font = QFont(painter->font());
    QString text = tr("Paused");
    font.setPixelSize(Dimensions::pauseOverlaySize / 4 * Dimensions::scaleWithTextLength("Paused", text));
    painter->setFont(font);
    painter->setPen(QPen(Qt::gray, 5));
    painter->drawText(
        QRectF(offset, offset, Dimensions::pauseOverlaySize, Dimensions::pauseOverlaySize),
        Qt::AlignCenter | Qt::AlignVCenter,
        text
    );

    // Draw the help text.
    font = QFont(painter->font());
    text = tr("Press %1 to continue!").arg("P");
    font.setPixelSize(20 * Dimensions::scaleWithTextLength(QString("Press %1 to continue").arg("P"), text));
    painter->setFont(font);
    painter->setPen(QPen(Qt::gray, 2));
    painter->drawText(
        QRectF(offset, offset + Dimensions::pauseOverlaySize * 3 / 4, Dimensions::pauseOverlaySize, Dimensions::pauseOverlaySize / 4),
        Qt::AlignCenter | Qt::AlignVCenter,
        text
    );
}
