# $Id: Sudoku_debug.pro 297 2008-05-30 20:16:20Z wimleers $

CONFIG += debug

!include(Sudoku_shared.pri) {
  warning("Could not include Sudoku_shared.pri, which is a necessity!")
}
