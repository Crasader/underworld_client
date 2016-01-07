//
//  TowerData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/5.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "TowerData.h"
#include "DataManager.h"
#include "cocostudio/CocoStudio.h"
#include "TowerLocalData.h"

using namespace cocostudio;

TowerData::TowerData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
{
    
}

TowerData::~TowerData()
{
    
}

const TowerLocalData* TowerData::getLocalData() const
{
    return DataManager::getInstance()->getTowerData(getId());
}
