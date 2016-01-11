//
//  SoldierManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "SoldierManager.h"
#include "cocostudio/CocoStudio.h"
#include "Utils.h"
#include "SoldierData.h"
#include "SoldierQualityData.h"
#include "GearData.h"

using namespace std;
using namespace cocostudio;

SoldierManager::SoldierManager()
{
}

SoldierManager::~SoldierManager()
{
    Utils::clearMap(_soldiers);
}

void SoldierManager::init(const rapidjson::Value& jsonDict)
{
    
}

void SoldierManager::requestForDetail(int id)
{
    
}

void SoldierManager::upgrade(int id)
{
    if (_soldiers.find(id) != _soldiers.end()) {
        SoldierData* data = _soldiers.at(id);
        if (data) {
            data->setLevel(data->getLevel() + 1);
        }
    } else {
        assert(false);
    }
}

void SoldierManager::recast(int id)
{
    if (_soldiers.find(id) != _soldiers.end()) {
        SoldierData* data = _soldiers.at(id);
        if (data) {
            const SoldierQualityData* qualityData = data->getQualityData();
            data->setQuality(qualityData->getLevel() + 1);
        }
    } else {
        assert(false);
    }
}

void SoldierManager::setGear(int id, int index, const GearData* gear)
{
    if (_soldiers.find(id) != _soldiers.end()) {
        SoldierData* data = _soldiers.at(id);
        if (data) {
            data->setGear(index, gear);
        }
    } else {
        assert(false);
    }
}

void SoldierManager::upgradeGear(int id, int index)
{
    if (_soldiers.find(id) != _soldiers.end()) {
        SoldierData* data = _soldiers.at(id);
        if (data) {
            const GearData* gear = data->getGearData(index);
            data->setGearLevel(index, gear->getLevel() + 1);
        }
    } else {
        assert(false);
    }
}

void SoldierManager::switchTalent(int id, int talent)
{
    if (_soldiers.find(id) != _soldiers.end()) {
        SoldierData* data = _soldiers.at(id);
        if (data) {
            // TODO
        }
    } else {
        assert(false);
    }
}

const SoldierData* SoldierManager::getSoldierData(int id)
{
    if (_soldiers.find(id) != _soldiers.end()) {
        return _soldiers.at(id);
    }
    
    return nullptr;
}
