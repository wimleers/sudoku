// $Id: PauseOverlayEventFilter.h 333 2008-06-07 13:22:48Z wimleers $


/**
 * PauseOverlayEventFilter class definition and implementation.
 *
 * @file PauseOverlayEventFilter.h
 * 
 * @author Wim Leers
 */


#ifndef PAUSEOVERLAYEVENTFILTER_H
#define PAUSEOVERLAYEVENTFILTER_H


#include <QObject>
#include <QEvent>
#include <QAction>
#include <QDebug>


class PauseOverlayEventFilter : public QObject {

    Q_OBJECT

public:
    PauseOverlayEventFilter(QObject * parent, QAction * pauseAction)
        : QObject(parent) { m_pauseAction = pauseAction; }

protected:
    bool eventFilter(QObject * obj, QEvent * event) {
        Q_UNUSED(obj);

        if (event->type() == QEvent::GraphicsSceneMouseDoubleClick) {
            m_pauseAction->setChecked(false);
            return true;     
        }
        else if (
               event->type() == QEvent::KeyPress
            || event->type() == QEvent::GraphicsSceneMouseMove
            || event->type() == QEvent::GraphicsSceneMousePress
            || event->type() == QEvent::GraphicsSceneMouseRelease
            || event->type() == QEvent::GraphicsSceneContextMenu
            || event->type() == QEvent::GraphicsSceneHoverEnter
            || event->type() == QEvent::GraphicsSceneHoverLeave
            || event->type() == QEvent::GraphicsSceneHoverMove
            || event->type() == QEvent::FocusIn
            || event->type() == QEvent::FocusOut
        ) {
                return true;
        }
        
        return false;
    }

private:
    QAction * m_pauseAction;
};

#endif // PAUSEOVERLAYEVENTFILTER_H
