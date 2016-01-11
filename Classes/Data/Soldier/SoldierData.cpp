//
//  SoldierData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "SoldierData.h"
#include "DataManager.h"
#include "cocostudio/CocoStudio.h"
#include "Utils.h"
#include "AttributeData.h"
#include "SoldierLocalData.h"
#include "SoldierUpgradeData.h"
#include "SoldierQualityData.h"
#include "SoldierTalentData.h"
#include "GearData.h"

using namespace std;
using namespace cocostudio;

SoldierData::SoldierData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
,_quality(0)
,_talent(0)
{
    _quality = DICTOOL->getIntValue_json(jsonDict, "quality");
    _talent = DICTOOL->getIntValue_json(jsonDict, "talent");
    // TODO: gears
}

SoldierData::~SoldierData()
{
    Utils::clearMap(_attributes);
    Utils::clearMap(_gears);
}

#pragma mark - getters
const map<int, AttributeData *>& SoldierData::getAttributes() const
{
    return _attributes;
}

const AttributeData* SoldierData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}

const SoldierLocalData* SoldierData::getLocalData() const
{
    return DataManager::getInstance()->getSoldierData(getId());
}

const SoldierUpgradeData* SoldierData::getUpgradeData() const
{
    return DataManager::getInstance()->getSoldierUpgradeData(getId(), getLevel());
}

const SoldierQualityData* SoldierData::getQualityData() const
{
    return DataManager::getInstance()->getSoldierQualityData(getId(), _quality);
}

const SoldierTalentData* SoldierData::getTalentData() const
{
    return DataManager::getInstance()->getSoldierTalentData(getId(), _talent);
}

const GearData* SoldierData::getGearData(int index) const
{
    if (_gears.find(index) != _gears.end()) {
        return _gears.at(index);
    }
    
    return nullptr;
}

#pragma mark - setters
void SoldierData::setLevel(int level)
{
    setLevel(level);
    calculateTotalAttributes();
}

void SoldierData::setQuality(int quality)
{
    _quality = quality;
    calculateTotalAttributes();
}

void SoldierData::setTalent(int talent)
{
    _talent = talent;
    calculateTotalAttributes();
}

void SoldierData::setGear(int index, const GearData* gear)
{
    if (_gears.find(index) != _gears.end()) {
        CC_SAFE_DELETE(_gears.at(index));
        _gears.at(index) = new (nothrow) GearData(*gear);
        calculateTotalAttributes();
    } else {
        assert(false);
    }
}

void SoldierData::setGearLevel(int index, int level)
{
    if (_gears.find(index) != _gears.end()) {
        GearData* data = _gears.at(index);
        data->setLevel(level);
        calculateTotalAttributes();
    } else {
        assert(false);
    }
}

#pragma mark - private
void SoldierData::calculateTotalAttributes()
{
    if (_attributes.size() > 0) {
        Utils::clearMap(_attributes);
    }
    
    map<int, int> attrs;
    AttributeData::calculateAttributes(getLocalData()->getAttributes(), attrs);
    AttributeData::calculateAttributes(getUpgradeData()->getAttributes(), attrs);
    AttributeData::calculateAttributes(getQualityData()->getAttributes(), attrs);
    AttributeData::calculateAttributes(getTalentData()->getAttributes(), attrs);
    for (map<int, GearData*>::const_iterator iter = _gears.begin(); iter != _gears.end(); ++iter) {
        AttributeData::calculateAttributes(iter->second->getAttributes(), attrs);
    }
    for (map<int, int>::const_iterator iter = attrs.begin(); iter != attrs.end(); ++iter) {
        const int attrId = iter->first;
        AttributeData* attr = new (nothrow) AttributeData(attrId, iter->second);
        _attributes.insert(make_pair(attrId, attr));
    }
}
