// $Id: InternalException.h 297 2008-05-30 20:16:20Z wimleers $


/**
 *  Internal Exception header file
 * 
 * @file InternalException.h
 * @author Bram Bonne
 */


#ifndef INTERNALEXCEPTION_H
#define INTERNALEXCEPTION_H

#include <string>
using namespace std;
#include "Exception.h"

class InternalException : public Exception {
    public:
        InternalException(string method, string description) : Exception("InternalException was thrown:\nIn " + method + " :\t" + description) {}
};

#endif
