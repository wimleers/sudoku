// $Id: FileIOException.h 345 2008-06-07 19:28:19Z wimleers $


/**
 * FileIOException class definition.
 *
 * @file FileIOException.h
 * @author Wim Leers
 */


#ifndef FILEIOEXCEPTION_H
#define FILEIOEXCEPTION_H

#include <string>
using namespace std;
#include "Exception.h"

class FileIOException : public Exception {
    public:
        FileIOException(const string & method, const string & description) : Exception("FileIOException was thrown:\nIn " + method + " :\t" + description) {}
};

#endif // FILEIOEXCEPTION_H
