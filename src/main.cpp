// $Id: main.cpp 345 2008-06-07 19:28:19Z wimleers $


/**
 * Main.
 *
 * @file main.cpp
 * @author Bram Bonn&eacute;
 * @author Wim Leers
 */


#include <iostream>
#include <string>
using namespace std;
#include "Exception.h"
#include "SudokuApp.h"


int main(int argc, char * argv[]) {
    QT_REQUIRE_VERSION(argc, argv, "4.3.0")

    try {
        SudokuApp app(argc, argv);
        return app.exec();
    }
    catch (Exception e) {
        cout << e.GetDescription() << endl;
        return 1;
    }
}


/**
 * @mainpage Trimesteroverschrijdend Project: Sudoku
 *
 * @section welcome Welkom bij de Doxygen documentatie voor het trimesteroverschrijdend project van Bram Bonné en Wim Leers
 *
 * Deze documentatie werd automatisch gegenereerd door middel van Doxygen. Het is handig om de structuur beter te begrijpen.
 *
 * @section authors Auteurs
 * @li Bram Bonn&eacute; (0623825)
 * @li Wim Leers (0623800)
 */
