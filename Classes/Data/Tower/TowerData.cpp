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
:_id(0)
,_uniqueId(0)
{
    _id = DICTOOL->getIntValue_json(jsonDict, "qid");
    _uniqueId = DICTOOL->getIntValue_json(jsonDict, "uid");
}

TowerData::~TowerData()
{
    
}

int TowerData::getUniqueId() const
{
    return _id;
}

const TowerLocalData* TowerData::getLocalData() const
{
    return DataManager::getInstance()->getTowerData(_id);
}
