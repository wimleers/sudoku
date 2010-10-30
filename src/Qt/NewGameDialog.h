// $Id: NewGameDialog.h 333 2008-06-07 13:22:48Z wimleers $


/**
 * New Game Dialog definition.
 *
 * @file NewGameDialog.h
 * @author Wim Leers
 */


#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H


#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSettings>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QtDebug>
#include "../Sudoku.h"


class NewGameDialog : public QDialog {

    Q_OBJECT

public:
    NewGameDialog(QWidget * parent = NULL) : QDialog(parent, Qt::Sheet) { setupUI(); }
    ~NewGameDialog(void);

signals:
    void newGame(int difficulty);

private slots:
	void accept(void);

private:
    QVBoxLayout * m_mainLayout;
    QHBoxLayout * m_firstSetting;
    QLabel * m_label1;
	QSpinBox * m_difficultyPicker;
    QPushButton * m_cancel, * m_start;
    int m_difficulty;

	void setupUI(void);
};


#endif // NEWGAMEDIALOG_H
