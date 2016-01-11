//
//  AbstractData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AbstractData.h"
#include "cocostudio/CocoStudio.h"

using namespace cocostudio;

AbstractData::AbstractData(const rapidjson::Value& jsonDict)
:_id(0)
,_uniqueId(0)
,_level(0)
{
    _id = DICTOOL->getIntValue_json(jsonDict, "id");
    _uniqueId = DICTOOL->getIntValue_json(jsonDict, "unique");
    _level = DICTOOL->getIntValue_json(jsonDict, "level");
}

AbstractData::AbstractData(const AbstractData& instance)
:_id(instance._id)
,_uniqueId(instance._uniqueId)
,_level(instance._level)
{
    
}

AbstractData::~AbstractData()
{
    
}

int AbstractData::getId() const
{
    return _id;
}

int AbstractData::getUniqueId() const
{
    return _uniqueId;
}

int AbstractData::getLevel() const
{
    return _level;
}

void AbstractData::setLevel(int level)
{
    _level = level;
}
