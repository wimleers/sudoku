// $Id: SudokuView.cpp 312 2008-05-31 17:06:00Z wimleers $


/**
 * Qt SudokuView implementation.
 *
 * @file SudokuView.cpp
 * @author Wim Leers
 */


#include "SudokuView.h"


//----------------------------------------------------------------------------
// Constructor.

SudokuView::SudokuView(SudokuScene * scene, QWidget * parent) : QGraphicsView(scene, parent), m_scene(scene) {
    // Performance.
    setRenderHints(
        QPainter::Antialiasing
        | QPainter::TextAntialiasing
        | QPainter::SmoothPixmapTransform
        | QPainter::HighQualityAntialiasing
    );
    setOptimizationFlags(
        QGraphicsView::DontClipPainter
        | QGraphicsView::DontAdjustForAntialiasing
        | QGraphicsView::DontSavePainterState
    );
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);


    // Use OpenGL for rendering when viable!
    #if defined(Q_OS_MACX)
        // The rendering is not 100% correct: borders are drawn thinner, the
        // board doesn't fit entirely in the view anymore and most annoyingly,
        // the text is seriously flawed.
        // When hovering like a maniac:
        // - QWidget uses 15-20% CPU time (maximum quality)
        // - QGLWidget uses 20-25% CPU time (maximum quality)
        // - QGLWidget uses 20-25% CPU time (default quality)
        // Obviously, QWidget is a better choice on Mac OS X.
        qDebug() << "[MACOSX] " << "Not using OpenGL for rendering. Qt's OpenGL bridge doesn't work well, not even in Leopard.";
    #elif defined(Q_OS_LINUX)
        // The rendering is not 100% correct: borders are drawn thinner, the
        // board doesn't fit entirely in the view anymore and most annoyingly,
        // the text is seriously flawed.
        qDebug() << "[LINUX]  " << "Not using OpenGL for rendering. Qt's OpenGL bridge doesn't work well, at least not in Ubuntu 8.04.";
    #elif defined(Q_OS_WIN_32)
        // Needs testing. Assume QGLWidget.
        qDebug() << "[WIN32]  " << "Using OpenGL for rendering. Needs testing.";
        setViewport(new QGLWidget());
    #endif


    // Appearance.
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);

    // Background.
    setBackgroundBrush(QColor(170, 218, 238));
}


//----------------------------------------------------------------------------
// Private methods.

void SudokuView::resizeEvent(QResizeEvent * event) {
    m_scene->resizeScene(event->size().width(), event->size().height());
}
