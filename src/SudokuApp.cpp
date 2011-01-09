// $Id: SudokuApp.cpp 298M 2010-10-29 18:42:52Z (local) $


/**
 * SudokuApp implementation.
 *
 * @file SudokuApp.cpp
 * @author Wim Leers
 */


#include "SudokuApp.h"


//----------------------------------------------------------------------------
// Constructor.

SudokuApp::SudokuApp(int & argc, char ** argv) : QApplication(argc, argv) {
    qDebug() << "[general]" << "Last update to source code was on $LastChangedDate: 2010-10-29 20:42:52 +0200 (Fri, 29 Oct 2010) $";
    qDebug() << "[general]" << "This application was compiled using Qt" << qVersion();

    // Set up translations.
    QTranslator * qtTranslator = new QTranslator();
    qtTranslator->load("qt_" + QLocale::system().name());
    installTranslator(qtTranslator);
    QTranslator * sudokuTranslator = new QTranslator();
    sudokuTranslator->load("translations/sudoku_" + QLocale::system().name());
    // For testing.
    //sudokuTranslator->load("translations/sudoku_nl");
    installTranslator(sudokuTranslator);

    // This simplifies the retrieval of application settings using QSettings.
    QCoreApplication::setOrganizationName("BW Games");
    QCoreApplication::setOrganizationDomain("bwgames.com");
    QCoreApplication::setApplicationName("Sudoku");

    // Force initialization of resources in case of a static build.
    //Q_INIT_RESOURCE(Sudoku);

    // Without this line, the application won't quit on Linux.
    connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quit()));

    m_mainWindow = new MainWindow();
    m_mainWindow->show();

    // Initialize & install the event logger.
    m_eventLogger = new QEventLogger("./events", m_mainWindow, true);
    this->installEventFilter(m_eventLogger);
}


//----------------------------------------------------------------------------
// Protected methods.

bool SudokuApp::event(QEvent * event) {
    #ifndef Q_OS_LINUX
        if (event->type() == QEvent::ApplicationDeactivate) {
            event->accept();
            m_mainWindow->deactivated();
            qDebug() << "[!LINUX]" << "Application was deactivated, game (if any) was paused.";
            return true;
        }
    #else
        Q_UNUSED(event)
        // QTBUG: The game won't be paused automatically in Linux, because the
        // QEvent::ApplicationDeactivate is triggered when
        // QEvent::WindowDeactivate should be triggered.
    #endif

    return false;
}
