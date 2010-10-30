REM $Id: build_windows_mingw.bat 348 2008-06-07 20:01:55Z wimleers $

@echo off

REM TODO: this should only be called when %DevEnvDir% or %VCINSTALLDIR% is not
REM yet set. If you call this too many times, your %PATH% variable will get too long!
call "%VS80COMNTOOLS%\vsvars32.bat"

cd src

echo "|-----------------------------|"
echo "| Building Sudoku for Windows |"
echo "|-----------------------------|"
echo "[1/3] Generating a make file ..."
qmake Sudoku_release.pro

echo "[2/3] Copying required DLLs ..."
copy /Y "%QTDIR%\bin\QtCore4.dll" release\
copy /Y "%QTDIR%\bin\QtGui4.dll" release\
copy /Y "%QTDIR%\bin\QtOpenGL4.dll" release\
copy /Y "%VCINSTALLDIR%\redist\x86\Microsoft.VC80.CRT\msvcp80.dll" release\
copy /Y "%VCINSTALLDIR%\redist\x86\Microsoft.VC80.CRT\msvcr80.dll" release\

REM For some reason, after make is called, the batch file *stops* running.
echo "[3/3] Building the project in release mode ..."
make release

cd ..
echo "All done! Sudoku is ready for release! You still have to package it though. Please look in the release directory."
