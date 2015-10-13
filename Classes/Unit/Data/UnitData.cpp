//
//  UnitData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "UnitData.h"
#include "tinyxml2/tinyxml2.h"

UnitData::UnitData(tinyxml2::XMLElement *xmlElement)
:TowerData(xmlElement)
,_movement_speed(0)
,_corpse_decay_time(0)
{
    
}

UnitData::~UnitData()
{
    
}

int UnitData::movement_speed() const
{
    return _movement_speed;
}

float UnitData::corpse_decay_time() const
{
    return _corpse_decay_time;
}