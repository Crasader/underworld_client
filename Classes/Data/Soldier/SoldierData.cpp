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

using namespace cocostudio;

SoldierData::SoldierData(const rapidjson::Value& jsonDict)
:_id(0)
,_uniqueId(0)
{
    _id = DICTOOL->getIntValue_json(jsonDict, "qid");
    _uniqueId = DICTOOL->getIntValue_json(jsonDict, "uid");
}

SoldierData::~SoldierData()
{
    
}

int SoldierData::getUniqueId() const
{
    return _id;
}

const SoldierLocalData* SoldierData::getLocalData() const
{
    return DataManager::getInstance()->getSoldierData(_id);
}
