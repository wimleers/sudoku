// $Id: FileIO.h 297 2008-05-30 20:16:20Z wimleers $


/**
 * FileIO template class definition.
 *
 * @file FileIO.h
 * @author Wim Leers
 */


#ifndef FILEIO_H
#define FILEIO_H

#include <fstream>
#include <string>
#include <queue>
using namespace std;
#include "FileIOException.h"

enum Format { FORMAT_CSV };

#define FILEIO_ROW deque<string>
#define FILEIO_TABLE deque< FILEIO_ROW >

class FileIO {
    public:
        FileIO(const string & fileName = "db", Format f = FORMAT_CSV) { m_fileName = fileName; m_format = f; }
        ~FileIO(void) {}
        Format GetFormat(void) const { return m_format; }
        void SetFormat(Format f) { m_format = f; }
        const string & GetFileName(void) const { return m_fileName; }
        void SetFileName(const string & fileName) { m_fileName = fileName; }
        FILEIO_TABLE GetTable(void) const { return m_table; }
        void SetTable(FILEIO_TABLE table ) { m_table = table; }
        void Open(void);
        bool Save(void);
        void Erase(void);
        
    private:
        Format m_format;
        string m_fileName;
        FILEIO_TABLE m_table;
        FILEIO_ROW csvReadRow(ifstream & fs);
        void cvsWriteRow(ofstream & fs, FILEIO_ROW row);
};

#endif
