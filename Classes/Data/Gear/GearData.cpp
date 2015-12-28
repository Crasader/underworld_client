//
//  GearData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearData.h"
#include "Utils.h"
#include "AttributeData.h"

using namespace std;

GearData::GearData(const rapidjson::Value& jsonDict)
:_id(0)
{
    
}

GearData::~GearData()
{
    Utils::clearMap(_attributes);
}

int GearData::getId() const
{
    return _id;
}

const AttributeData* GearData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}