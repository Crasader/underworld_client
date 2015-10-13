//
//  Coordinate.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef Coordinate_h
#define Coordinate_h

class Coordinate
{
public:
    Coordinate();
    Coordinate(int _x, int _y);
    Coordinate(const Coordinate& c);
    
    inline bool operator==(const Coordinate& c) const;
    inline bool operator<(const Coordinate& c) const;
    inline const Coordinate operator+(const Coordinate& c) const;
    inline const Coordinate operator-(const Coordinate& c) const;
    
    int x;
    int y;
};

inline bool Coordinate::operator==(const Coordinate& c) const
{
    return x == c.x && y == c.y;
}

inline bool Coordinate::operator<(const Coordinate& c) const
{
    return x < c.x || (x == c.x && y < c.y);
}

inline const Coordinate Coordinate::operator+(const Coordinate& c) const
{
    Coordinate result(*this);
    result.x += c.x;
    result.y += c.y;
    return result;
}

inline const Coordinate Coordinate::operator-(const Coordinate& c) const
{
    Coordinate result(*this);
    result.x -= c.x;
    result.y -= c.y;
    return result;
}

#endif /* Coordinate_h */
