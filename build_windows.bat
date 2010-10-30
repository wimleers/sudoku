REM $Id: build_windows.bat 342 2008-06-07 16:59:37Z wimleers $

@echo off

REM TODO: this should only be called when %DevEnvDir% or %VCINSTALLDIR% is not
REM yet set. If you call this too many times, your %PATH% variable will get too long!
call "%VS80COMNTOOLS%\vsvars32.bat"

cd src

echo "|-----------------------------|"
echo "| Building Sudoku for Windows |"
echo "|-----------------------------|"
echo "[1/3] Generating a Visual Studio project file ..."
qmake Sudoku_release.pro -t vcapp

echo "[2/3] Building the project in release mode ..."
set PWD=%~dp0
"%DevEnvDir%\devenv.exe" "%PWD%src\Sudoku.vcproj" /Clean
"%DevEnvDir%\devenv.exe" "%PWD%src\Sudoku.vcproj" /Build "Release|Win32"

echo "[3/3] Copying required DLLs ..."
copy /Y "%QTDIR%\bin\QtCore4.dll" release\
copy /Y "%QTDIR%\bin\QtGui4.dll" release\
copy /Y "%QTDIR%\bin\QtOpenGL4.dll" release\
copy /Y "%VCINSTALLDIR%\redist\x86\Microsoft.VC80.CRT\msvcp80.dll" release\
copy /Y "%VCINSTALLDIR%\redist\x86\Microsoft.VC80.CRT\msvcr80.dll" release\

cd ..
echo "All done! Sudoku is ready for release! You still have to package it though. Please look in the release directory."
