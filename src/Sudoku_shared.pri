# $Id: Sudoku_shared.pri 312M 2010-10-29 18:45:26Z (local) $


message(Qt version: $$[QT_VERSION])
CONFIG(release, debug|release) {
  message("--- Sudoku project file generation (release mode) ---")
} else {
  message("--- Sudoku project file generation (debug mode) ---")
}


# Basic settings.
message("Basic settings...")
TEMPLATE = app
CONFIG += qt thread resources warn_on
QT += printsupport
OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
TARGET = Sudoku


# Input files.
message("Input files...")
HEADERS += Board.h \
           Exception.h \
           FileIO.h \
           FileIOException.h \
           InternalException.h \
           Sudoku.h \
           BoardGenerator.h \
           SudokuApp.h \
           Qt/Dimensions.h \
           Qt/MainWindow.h \
           Qt/NewGameDialog.h \
           Qt/PauseOverlay.h \
           Qt/PauseOverlayEventFilter.h \
           Qt/SudokuGame.h \
           Qt/SudokuView.h \
           Qt/SudokuScene.h \
           Qt/SudokuElement.h \
           Qt/SudokuHUD.h
SOURCES += Board.cpp \
           FileIO.cpp \
           main.cpp \
           Sudoku.cpp \
           BoardGenerator.cpp \
           SudokuApp.cpp \
           Qt/MainWindow.cpp \
           Qt/NewGameDialog.cpp \
           Qt/PauseOverlay.cpp \
           Qt/SudokuGame.cpp \
           Qt/SudokuView.cpp \
           Qt/SudokuScene.cpp \
           Qt/SudokuElement.cpp \
           Qt/SudokuHUD.cpp
RESOURCES += resources/Sudoku.qrc
TRANSLATIONS += translations/sudoku_nl.ts \
                translations/sudoku_fr.ts

# Linux platform-specific settings.
linux-g++ {
  message("Building for Linux (makefile) ...")
}


# Mac OS X platform-specific settings.
macx {
  message("Building for Mac OS X (Xcode project file) ...")

  # Application icon.
  ICON = resources/macx/icon.icns

  # Always create an .app bundle.
  CONFIG += app_bundle
  
  # We want a universal binaries as releases.
  CONFIG(release, debug|release) {
    message("Configured to build a universal binary")
    CONFIG += x86 x86_64
  }
}


# Windows platform-specific settings.
win32 {
  message("Building for Windows (Visual Studio project file) ...")

  # Application icon.
  RC_FILE = resources/win32/icon.rc

  # Enable manifest embedding (because we're not generating DLLs). Enabling
  # this when the windows subystem is used, results in linker errors.
  #CONFIG += embed_manifest_exe

  # Using OpenGL on Windows works fine, as long as you don't mind a console
  # window (subsystem = WINDOWS in linker settings results in linker errors).
  QT += opengl

  CONFIG(release, debug|release) {
    message("Configured to be a windowed application")
    CONFIG += windows
  }

  # This does not have *any* effect! You still have to set the linker's
  # subsystem to console manually.
  CONFIG(debug, debug|release) {
    message("Configured to be a console application")
    CONFIG += console
  }
}

