//
//  HeroData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "HeroData.h"
#include "DataManager.h"
#include "cocostudio/CocoStudio.h"
#include "HeroLocalData.h"
#include "HeroUpgradeData.h"

using namespace cocostudio;

HeroData::HeroData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
{
    
}

HeroData::~HeroData()
{
    
}

const HeroLocalData* HeroData::getLocalData() const
{
    return DataManager::getInstance()->getHeroData(getId());
}

const HeroUpgradeData* HeroData::getUpgradeData() const
{
    return DataManager::getInstance()->getHeroUpgradeData(getId(), getLevel());
}
