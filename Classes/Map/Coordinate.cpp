//
//  Coordinate.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "Coordinate.h"
#include "Global.h"

Coordinate::Coordinate()
:x(INVALID_VALUE)
,y(INVALID_VALUE)
{}

Coordinate::Coordinate(int _x, int _y)
:x(_x)
,y(_y)
{}

Coordinate::Coordinate(const Coordinate& c)
:x(c.x)
,y(c.y)
{}
