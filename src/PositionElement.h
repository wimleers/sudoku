// $Id: PositionElement.h 297 2008-05-30 20:16:20Z wimleers $


/**
 * PositionElement Class definition.
 * This class holds a position on the board and an element associated with it.
 * 
 * @file PositionElement.h
 * @author Bram Bonne
 */


#ifndef POSITIONELEMENT_H
#define POSITIONELEMENT_H

class PositionElement {
public:
    PositionElement(int x, int y, char e) : m_x(x), m_y(y), m_e(e) { }
    PositionElement(const PositionElement& other) {
        m_x = other.m_x;
        m_y = other.m_y;
        m_e = other.m_e;
    }
    int GetX(void) { return m_x; }
    int GetY(void) { return m_y; }
    char GetE(void) { return m_e; }
    
private:
    int m_x;
    int m_y;
    char m_e;
};

#endif
