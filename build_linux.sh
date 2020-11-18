# $Id: build_linux.sh 381 2008-06-10 12:20:37Z gooz $

#!/bin/sh

cd src

echo "|---------------------------|"
echo "| Building Sudoku for Linux |"
echo "|---------------------------|"
echo "[1/3] Generating a make file ..."
qmake Sudoku_release.pro

echo "[2/3] Building the project in release mode ..."
make clean > /dev/null
make > /dev/null

echo "[3/3] Packaging into a tar.gz ..."
cd ..
tar -czf Sudoku.tar.gz -C src Sudoku

echo "All done! ./Sudoku.tar.gz is ready for release!"
