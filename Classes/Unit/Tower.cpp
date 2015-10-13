//
//  Tower.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "Tower.h"

Tower::Tower(int TowerId)
:_id(INVALID_VALUE)
,_towerData(nullptr)
{
    
}

Tower::~Tower()
{
    
}

int Tower::getId() const
{
    return _id;
}

const TowerData* Tower::getTowerData() const
{
    return _towerData;
}