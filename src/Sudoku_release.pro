# $Id: Sudoku_release.pro 297 2008-05-30 20:16:20Z wimleers $

CONFIG += release

!include(Sudoku_shared.pri) {
  warning("Could not include Sudoku_shared.pri, which is a necessity!")
}
