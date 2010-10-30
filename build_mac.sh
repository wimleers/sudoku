# $Id: build_mac.sh 339M 2010-05-02 14:24:15Z (local) $

#!/bin/sh

cd src

echo "|------------------------------|"
echo "| Building Sudoku for Mac OS X |"
echo "|------------------------------|"
echo "[1/6] Generating an Xcode project file ..."
qmake Sudoku_release.pro

echo "[2/6] Building the project using the Release configuration ..."
xcodebuild -project Sudoku.xcodeproj -configuration Release clean build > /dev/null

echo "[3/6] Copy the necessary Qt frameworks into the application bundle ..."
deployqt Sudoku.app # You can get deployqt from http://labs.trolltech.com/blogs/2007/08/23/deploying-mac-applications-without-the-hassle/

echo "[4/6] Packaging into a DMG ..."
mkdir dmgtmp
mv Sudoku.app dmgtmp/
hdiutil create -srcfolder dmgtmp -volname "Sudoku" ~/Desktop/Sudoku > /dev/null
mv dmgtmp/Sudoku.app ./
rm -rf dmgtmp

echo "[5/6] Mounting the created DMG ..."
hdiutil attach ~/Desktop/Sudoku.dmg > /dev/null

echo "[6/6] Showing the mounted DMG ..."
open /Volumes/Sudoku

cd ..
echo "All done! ~/Desktop/Sudoku.dmg is ready for release!"
