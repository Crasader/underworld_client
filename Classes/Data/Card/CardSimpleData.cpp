//
//  CardSimpleData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CardSimpleData.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

CardSimpleData::CardSimpleData(const rapidjson::Value& jsonDict)
:_level(0)
{
    _level = DICTOOL->getIntValue_json(jsonDict, "level");
    _name = DICTOOL->getStringValue_json(jsonDict, "name");
}

CardSimpleData::CardSimpleData(const CardSimpleData* instance)
:_level(instance->_level)
,_name(instance->_name) {}

CardSimpleData::~CardSimpleData()
{
    
}

const string& CardSimpleData::getName() const
{
    return _name;
}

int CardSimpleData::getLevel() const
{
    return _level;
}
