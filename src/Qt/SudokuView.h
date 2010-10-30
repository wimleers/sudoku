#ifndef SUDOKUVIEW_H
#define SUDOKUVIEW_H

#include <QGraphicsView>
#include <QResizeEvent>
#include "SudokuScene.h"
#include "SudokuElement.h"


// See SudokuView.cpp for details.
#if defined(Q_OS_MACX)
    // We don't use OpenGL for rendering on Mac OS X.
#elif defined(Q_OS_LINUX)
    // We don't use OpenGL for rendering on Linux.
#elif defined(Q_OS_WIN32)
    // We do use OpenGL for rendering on Windows.
    #include <QtOpenGL>
    #include <QGLWidget>
#endif


class SudokuView : public QGraphicsView {

    Q_OBJECT

public:
    SudokuView(SudokuScene * scene, QWidget * parent = 0);

private:
    virtual void resizeEvent(QResizeEvent * event);

    SudokuScene * m_scene;
};

#endif // SUDOKUVIEW_H
