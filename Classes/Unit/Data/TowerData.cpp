//
//  TowerData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "TowerData.h"
#include "tinyxml2/tinyxml2.h"

TowerData::TowerData(tinyxml2::XMLElement *xmlElement)
:_type(kUnit)
,_id(INVALID_VALUE)
,_hp(0)
,_damage_min(0)
,_damage_max(0)
,_attack_speed(0)
,_attack_range(0)
,_attack_type(kAttackType_Normal)
,_sight(0)
,_collision_size(0)
{
    
}

TowerData::~TowerData()
{
    
}

UType TowerData::type() const
{
    return _type;
}

int TowerData::id() const
{
    return _id;
}

const std::string& TowerData::name() const
{
    return _name;
}

int TowerData::hp() const
{
    return _hp;
}

int TowerData::damage_min() const
{
    return _damage_min;
}

int TowerData::damage_max() const
{
    return _damage_max;
}

float TowerData::attack_speed() const
{
    return _attack_speed;
}

int TowerData::attack_range() const
{
    return _attack_range;
}

AttackType TowerData::attack_type() const
{
    return _attack_type;
}

int TowerData::sight() const
{
    return _sight;
}

int TowerData::collision_size() const
{
    return _collision_size;
}