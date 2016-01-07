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
#include "SoldierLocalData.h"
#include "SoldierUpgradeData.h"
#include "SoldierQualityData.h"
#include "SoldierTalentData.h"

using namespace cocostudio;

SoldierData::SoldierData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
,_quality(0)
,_talent(0)
{
    
}

SoldierData::~SoldierData()
{
    
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

