//
//  GearData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearData.h"
#include "Utils.h"

using namespace std;

GearData::GearData(const rapidjson::Value& jsonDict)
:_id(0)
{
    
}

GearData::~GearData()
{
    Utils::clearVector(_attributes);
}

int GearData::getId() const
{
    return _id;
}

const std::vector<GearAttribute *>& GearData::getAttributes() const
{
    return _attributes;
}