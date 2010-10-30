// $Id: Exception.h 297 2008-05-30 20:16:20Z wimleers $


/**
 * Exception class definition.
 *
 * @file Exception.h
 * @author Wim Leers
 */


#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
using namespace std;

class Exception {
public:
    Exception(const string & description) { m_description = description; }
    string GetDescription(void) { return m_description; }
protected:
    string m_description;
};

#endif // EXCEPTION_H
