//
//  Unit.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "Unit.h"

Unit::Unit(int unitId)
:_id(INVALID_VALUE)
,_unitData(nullptr)
{
    
}

Unit::~Unit()
{
    
}

int Unit::getId() const
{
    return _id;
}

const UnitData* Unit::getUnitData() const
{
    return _unitData;
}