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

using namespace cocostudio;

HeroData::HeroData(const rapidjson::Value& jsonDict)
:_id(0)
,_uniqueId(0)
{
    _id = DICTOOL->getIntValue_json(jsonDict, "qid");
    _uniqueId = DICTOOL->getIntValue_json(jsonDict, "uid");
}

HeroData::~HeroData()
{
    
}

int HeroData::getUniqueId() const
{
    return _id;
}

const HeroLocalData* HeroData::getLocalData() const
{
    return DataManager::getInstance()->getHeroData(_id);
}
