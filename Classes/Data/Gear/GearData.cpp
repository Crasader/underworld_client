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
#include "DataManager.h"
#include "cocostudio/CocoStudio.h"
#include "GearLocalData.h"
#include "GearUpgradeData.h"

using namespace std;

GearData::GearData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
{
    
}

GearData::~GearData()
{
    Utils::clearMap(_attributes);
}

GearQuality GearData::getQuality() const
{
    return kGearQuality_White;
}

const map<int, AttributeData *>& GearData::getAttributes() const
{
    return _attributes;
}

const AttributeData* GearData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}

const GearLocalData* GearData::getLocalData() const
{
    return DataManager::getInstance()->getGearData(getId());
}

const GearUpgradeData* GearData::getUpgradeData() const
{
    return DataManager::getInstance()->getGearUpgradeData(getId(), getLevel());
}
