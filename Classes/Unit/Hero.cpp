//
//  Hero.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "Hero.h"

Hero::Hero(int heroId)
:Unit(heroId)
,_level(INVALID_VALUE)
{
    
}

Hero::~Hero()
{
    
}

int Hero::getLevel() const
{
    return _level;
}