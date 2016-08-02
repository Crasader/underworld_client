//
//  CardSimpleData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "CardSimpleData.h"

using namespace std;

CardSimpleData::CardSimpleData(const rapidjson::Value& jsonDict)
:_level(0)
{
    
}

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
