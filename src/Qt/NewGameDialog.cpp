// $Id: NewGameDialog.cpp 306 2008-05-31 14:51:10Z wimleers $


/**
 * New Game Dialog implementation.
 *
 * @file NewGameDialog.cpp
 * @author Wim Leers
 */


#include "NewGameDialog.h"


//----------------------------------------------------------------------------
// Constructor & destructor.

NewGameDialog::~NewGameDialog(void) {
    // Is automatically deleted, because it's a child of of the main widget.
    //delete m_mainLayout;
    delete m_firstSetting;
    delete m_label1;
    delete m_difficultyPicker;
    delete m_cancel;
    delete m_start;
}


//----------------------------------------------------------------------------
// Private slots.

/**
 * The dialog was accepted. Emit the newGame() signal.
 */
void NewGameDialog::accept(void) {
    m_difficulty = m_difficultyPicker->value();
    
    QSettings settings;
    settings.setValue("difficulty", m_difficulty);    
    
    close();

    emit newGame(m_difficulty);
}


//----------------------------------------------------------------------------
// Private methods.

/**
 * Create the UI.
 */
void NewGameDialog::setupUI(void) {
    setContextMenuPolicy(Qt::NoContextMenu);
    setModal(true);

    QSettings settings;

    // Configure layouts.
    m_mainLayout = new QVBoxLayout(this);
    m_firstSetting = new QHBoxLayout();
    m_mainLayout->addLayout(m_firstSetting);
    
    // Set up the buttons.
    QDialogButtonBox * buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    m_cancel = new QPushButton(tr("&Cancel"));
    buttonBox->addButton(m_cancel, QDialogButtonBox::RejectRole);
    m_start = new QPushButton(tr("&Start!"));
    m_start->setDefault(true);
    buttonBox->addButton(m_start, QDialogButtonBox::AcceptRole);
    m_mainLayout->addWidget(buttonBox);

    // Difficulty label.
    m_label1 = new QLabel(this);
    m_label1->setText(tr("Difficulty level"));
    m_firstSetting->addWidget(m_label1);
    m_firstSetting->setAlignment(m_label1, Qt::AlignLeft);

    // Difficulty spinbox.
    m_difficultyPicker = new QSpinBox(this);
    m_difficultyPicker->setMinimum(1);
    m_difficultyPicker->setMaximum(Sudoku::NumLevels());
    m_difficultyPicker->setSingleStep(1);
    m_difficultyPicker->setValue(settings.value("difficulty", 1).toInt());
    m_firstSetting->addWidget(m_difficultyPicker);
    m_firstSetting->setAlignment(m_difficultyPicker, Qt::AlignRight);

    // Connect the signals from the buttonbox to the NewGameDialog.
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}
