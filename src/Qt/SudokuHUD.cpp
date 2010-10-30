// $Id: SudokuHUD.cpp 350 2008-06-07 20:52:53Z wimleers $


/**
 * Qt SudokuHUD widget implementation.
 *
 * @file SudokuHUD.cpp
 * @author Wim Leers
 */


#include "SudokuHUD.h"


//----------------------------------------------------------------------------
// Constructor & destructor.

SudokuHUD::SudokuHUD(void) {
    m_gameLoaded = false;

    m_duration = 0;
    m_validityEnabled = false;
    m_solvabilityEnabled = false;
    m_statsEnabled = false;

    m_calculating = false;
    
    m_animTimer = new QTimer(this);
    connect(m_animTimer, SIGNAL(timeout()), this, SLOT(repaintSpinner()));
}

SudokuHUD::~SudokuHUD(void) {
    m_animTimer->stop();
    disconnect(m_animTimer, SIGNAL(timeout()), this, SLOT(repaintSpinner()));
    delete m_animTimer;
}


//----------------------------------------------------------------------------
// Public methods.

/**
 * Implementation of the pure virtual boundingRect() method.
 */
QRectF SudokuHUD::boundingRect(void) const {
    return QRectF(0, 0, Dimensions::HUDWidth, Dimensions::HUDHeight);
}

/**
 * Implementation of the pure virtual paint() method.
 */
void SudokuHUD::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) {
    Q_UNUSED(widget);
    
    // Only draw the part that is exposed to the user. From:
    // http://thesmithfam.org/blog/2007/02/03/qt-improving-qgraphicsview-performance/.
    painter->setClipRect(option->exposedRect);

    int m = Dimensions::margin; // Marge.
    int c = 2 * m;              // Corner size.
    int o = 2;                  // Offset.
    int width  = Dimensions::HUDWidth - o * 2;
    int height = Dimensions::HUDHeight - o * 2;

    // Create the painter path.
    QPainterPath roundRectPath;    
    roundRectPath.moveTo(o + width, o + c);
    roundRectPath.arcTo(o + width - c, o, c, c, 0.0, 90.0);                 // Top right arc.
    roundRectPath.lineTo(o + c, o);                                         // Line to top left arc.
    roundRectPath.arcTo(o, o, c, c, 90.0, 90.0);                            // Top left arc.
    roundRectPath.lineTo(o, o + height - c);                                // Line to bottom left arc.
    roundRectPath.arcTo(o, o + height - c, c, c, 180.0, 90.0);              // Bottom left arc.
    roundRectPath.lineTo(o + width - c, o + height);                        // Line to bottom right arc.
    roundRectPath.arcTo(o + width - c , o + height - c, c, c, 270.0, 90.0); // Bottom right arc
    roundRectPath.lineTo(o + width, o + c);
    roundRectPath.closeSubpath();

    // Use a gradient for the brush.
    painter->setBrush(getBackgroundGradient());
    
    // Actually draw the box!
    painter->setPen(QPen(QColor(56, 165, 211), 2));
    painter->drawPath(roundRectPath);

    // Smaller pen from now on.
    painter->setPen(QPen(QColor(56, 165, 211), 1));


    // Debug: show the bounding rects.
    /*
    painter->setPen(QPen(QColor(56, 165, 211), 1));
    painter->drawRect(getBoundingRectForTimer());
    painter->drawRect(getBoundingRectForValidity());
    painter->drawRect(getBoundingRectForSpinner());
    painter->drawRect(getBoundingRectForSpinnerText());
    painter->drawRect(getBoundingRectForSolvability());
    painter->drawRect(getBoundingRectForStats());
    */

    // Timer.
    if (m_gameLoaded) {
        QFont font = QFont(painter->font());
        font.setPixelSize(ceil(0.8 * 4 * m));
        painter->setFont(font);
        painter->drawText(
            getBoundingRectForTimer(),
            Qt::AlignCenter | Qt::AlignVCenter,
            QString(secondsToString(m_duration))
        );
    }

    // Validity.
    if (m_validityEnabled && m_gameLoaded) {
        double size = 0.55;
        size += (m_validity) ? 0.15 : 0.0;
        QFont font = QFont(painter->font());
        font.setPixelSize(ceil(size * 4 * m));
        font.setBold(true);
        painter->setFont(font);
        painter->setPen((m_validity) ? QColor(0, 202, 73) : QColor(226, 6, 30));
        painter->drawText(
            getBoundingRectForValidity(),
            Qt::AlignCenter | Qt::AlignVCenter,
            (m_validity) ? tr("Valid") : tr("Invalid")
        );
    }

    // Spinner and spinner text.
    if (m_calculating) {
        painter->setPen(QPen(QColor(56, 165, 211), 1));
        painter->setBrush(QColor(56, 165, 211));

        // Spinner.
        static int offset = 0;
        offset += 5;
        QRect r = getBoundingRectForSpinner();
        for (int i = 0; i < 8; i++)
            painter->drawPie(r, (offset + i * 45) * 16, 30 * 16);

        // Spinner text.
        QFont font = QFont(painter->font());
        font.setPixelSize(ceil(1.5 * m));
        font.setBold(false);
        painter->setFont(font);
        painter->drawText(
            getBoundingRectForSpinnerText(),
            Qt::AlignCenter | Qt::AlignTop,
            tr("Generating ...")
        );
    }


    // Solvability.
    if (m_solvabilityEnabled && m_gameLoaded) {
        double size = 0.4;
        size += (m_solvability) ? 0.15 : 0.0;
        QFont font = QFont(painter->font());
        font.setPixelSize(ceil(size * 4 * m));
        font.setBold(true);
        painter->setFont(font);
        painter->setPen((m_solvability) ? QColor(0, 202, 73) : QColor(226, 6, 30));
        painter->drawText(
            getBoundingRectForSolvability(),
            Qt::AlignCenter | Qt::AlignVCenter,
            (m_solvability) ? tr("Solvable") : tr("Unsolvable")
        );
    }
    
    // Stats.
    if (m_statsEnabled && m_gameLoaded) {
        QFont font = QFont(painter->font());
        font.setPixelSize(ceil(0.24 * 4 * m));
        font.setBold(false);
        painter->setFont(font);
        painter->setPen(QColor(80, 80, 80));
        painter->drawText(
            getBoundingRectForStats(),
            Qt::AlignLeft | Qt::AlignVCenter,
            tr("Generated: %1\n\nCompleted: %2\n\nRemaining: %3").arg(m_generated).arg(m_completed).arg(m_remaining)
        );
    }
}


//----------------------------------------------------------------------------
// Public slots.

void SudokuHUD::duration(unsigned int duration) {
    m_duration = duration;
    update(getBoundingRectForTimer());
}

void SudokuHUD::validity(bool validity) {
    m_validity = validity;
    update(getBoundingRectForValidity());
}

void SudokuHUD::solvability(bool solvability) {
    m_solvability = solvability;
    update(getBoundingRectForSolvability());
}

void SudokuHUD::stats(int generated, int completed, int remaining) {
    m_generated = generated;
    m_completed = completed;
    m_remaining = remaining;
    update(getBoundingRectForStats());
}

void SudokuHUD::calculating(bool calculating) {
    if (calculating)
        m_animTimer->start(50);
    else {
        m_animTimer->stop();
        // QT BUG: For some odd reason, an artifact is shown after the spinner
        // stops. Simply repainting the entire HUD fixes that.
        update(QRect(0, 0, Dimensions::HUDWidth, Dimensions::HUDHeight));
    }
    m_calculating = calculating;
    update(getBoundingRectForSpinner().united(getBoundingRectForSpinnerText()));
}


//----------------------------------------------------------------------------
// Private slots.

void SudokuHUD::repaintSpinner(void) {
    // QT BUG: in theory, getBoundingRectForSpinner() should be sufficient,
    // since the text doesn't have to be redrawn. However, when you do that,
    // there's a glitch: it seems that, due to rounding errors, Qt sometimes
    // renders a bit outside of the given bounding rect, so there'll be some
    // artifacts. So we slightly increase the size of the bounding rectangle.
    QRect r = getBoundingRectForSpinner();
    r.adjust(-5, -5, 10, 10);
    update(r);
}


//----------------------------------------------------------------------------
// Private methods.

QRect SudokuHUD::getBoundingRectForTimer(void) const {
    int m = Dimensions::margin;
    int w = Dimensions::HUDWidth - 2 * m;
    int h = 4 * m;
    return QRect(m, m, w, h);
}

QRect SudokuHUD::getBoundingRectForValidity(void) const {
    int m = Dimensions::margin;
    int y = 2.5 * Dimensions::elementSize + m / 2;
    int h = Dimensions::elementSize;
    int w = Dimensions::HUDWidth - 2 * m;
    
    return QRect(m, y, w, h);
}

QRect SudokuHUD::getBoundingRectForSolvability(void) const {
    int m = Dimensions::margin;
    int y = 5.5 * Dimensions::elementSize + m + m / 2;
    int h = Dimensions::elementSize;
    int w = Dimensions::HUDWidth - 2 * m;
    
    return QRect(m, y, w, h);
}

QRect SudokuHUD::getBoundingRectForStats(void) const {
    int m = Dimensions::margin;
    int h = 1.5 * Dimensions::elementSize;
    int w = Dimensions::HUDWidth - 5 * m;

    return QRect(2.5 * m, Dimensions::HUDHeight - m - h, w, h);
}

QRect SudokuHUD::getBoundingRectForSpinner(void) const {
    int m = Dimensions::margin;
    int y = 3.5 * Dimensions::elementSize + 2.5 * m;
    int s = Dimensions::HUDWidth - 10 * m;
    
    return QRect(5 * m, y, s, s);
}

QRect SudokuHUD::getBoundingRectForSpinnerText(void) const {
    QRect spinner = getBoundingRectForSpinner();
    int m = Dimensions::margin;
    int spinnerSize = Dimensions::HUDWidth - 10 * m;
    int y = spinner.y() + spinnerSize + m;
    int w = Dimensions::HUDWidth - 4 * m;
    
    return QRect(2 * m, y, w, m * 2);
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
QString SudokuHUD::secondsToString(unsigned int numSeconds) const {
    unsigned int minutes = numSeconds / 60;
    unsigned int seconds = numSeconds % 60;

    QString secondsString = QString::number(seconds);
    if (secondsString.length() == 1)
        secondsString = "0" + secondsString;

    return QString::number(minutes) + ":" + secondsString;
}

/**
 * Get the background gradient. This allows us to generate the gradient only
 * *once*!
 *
 * @return
 *   A gradient object.
 */
QLinearGradient SudokuHUD::getBackgroundGradient(void) {
    static bool generated = false;
    static QLinearGradient gradient;

    if (!generated) {
        // These 3 variables are essentially duplicates from SudokuHUD::paint().
        int o = 2; // Offset.
        int width  = Dimensions::HUDWidth - o * 2;
        int height = Dimensions::HUDHeight - o * 2;

        gradient = QLinearGradient (QPointF(width / 2, 0), QPointF(width / 2, height));
        gradient.setColorAt(0, QColor(223, 243, 252)); // Light blue at the top.
        gradient.setColorAt(0.4, Qt::white);           // White just above the center.
        gradient.setColorAt(1, QColor(223, 243, 252)); // Light blue at the bottom.

        generated = true;
    }

    return gradient;
}
