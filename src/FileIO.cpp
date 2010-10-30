// $Id: FileIO.cpp 297 2008-05-30 20:16:20Z wimleers $


// @TODO: Add more exceptions.


/**
 * FileIO class definition.
 *
 * @file FileIO.cpp
 * @author Wim Leers
 */


#include "FileIO.h"


//----------------------------------------------------------------------------
// Public methods.

/**
 * Open (and read from) a file.
 */
void FileIO::Open(void) {
    ifstream fs(m_fileName.c_str(), ios::in);
    FILEIO_ROW row;

    // Make sure the table is empty.
    m_table.erase(m_table.begin(), m_table.end());

    if (fs.is_open()) {
        while (!fs.eof()) {
            switch (m_format) {
                case FORMAT_CSV:
                    row = csvReadRow(fs);
                    if (row.size())
                        m_table.push_back(row);
                    break;
            }
        }
        fs.close();
    }
}

/**
 * Save a file.
 */
bool FileIO::Save(void) {
    ofstream fs(m_fileName.c_str(), ios::out);
    FILEIO_ROW row;
    FILEIO_TABLE::iterator it;

    if (fs.is_open()) {
        switch (m_format) {
            case FORMAT_CSV:
                for (it = m_table.begin(); it != m_table.end(); it++)
                    cvsWriteRow(fs, *it);
                break;
        }
        fs.close();
        return true;
    }
    else
        throw(FileIOException(
            "FileIO::Open()",
            "The file " + m_fileName + " could not be opened for writing."
        ));


}

/**
 * Erase the file.
 */
void FileIO::Erase(void) {
    // Erase the tabular representation of the file in the memory.
    m_table.erase(m_table.begin(), m_table.end());

    // Erase the file itself.
    ofstream f(m_fileName.c_str(), ios::trunc);
    f.close();
}


//----------------------------------------------------------------------------
// Private methods.

/**
 * Reads a row of strings from a filestream.
 *
 * @param fs
 *   Filestream that is opened in reading mode.
 * @return
 *   A row of strings.
 */
FILEIO_ROW FileIO::csvReadRow(ifstream & fs) {
    string line;
    unsigned int pos = 0, prevPos = 0;
    FILEIO_ROW row;

    // Read the next row (one line) of the file.
    getline(fs, line);

    // Put all fields in this row in a queue.
    while (line.length() > 0 && pos < line.npos) {
        // Ignore the comma.
        if (pos > 0)
            prevPos = pos + 1;

        pos = (unsigned int) line.find(',', prevPos);
        row.push_back(line.substr(prevPos, pos - prevPos));
    }
    return row;
}

/**
 * Writes a row of strings to a filestream.
 *
 * @param fs
 *   Filestream that is opened in writing mode.
 * @param row
 *   A row of strings.
 */
void FileIO::cvsWriteRow(ofstream & fs, FILEIO_ROW row) {
    FILEIO_ROW::iterator it, test;

    for (it = row.begin(); it != row.end(); it++) {
        fs << *it;

        // Test if we haven't reached the last column yet, and if so, print a
        // comma.
        test = it;
        test++;
        if (test != row.end())
            fs << ',';
    }
    fs << endl;
}
