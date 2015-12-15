//
//  AchievementData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AchievementData.h"
#include "DataManager.h"
#include "cocostudio/CocoStudio.h"

using namespace cocostudio;

AchievementData::AchievementData(const rapidjson::Value& jsonDict)
:_id(0)
,_progress(0)
,_status(0)
{
    _id = DICTOOL->getIntValue_json(jsonDict, "qid");
    _progress = DICTOOL->getIntValue_json(jsonDict, "progress");
    _status = DICTOOL->getIntValue_json(jsonDict, "status");
}

AchievementData::~AchievementData()
{
    
}

int AchievementData::getId() const
{
    return _id;
}

const AchievementLocalData* AchievementData::getLocalData() const
{
    return DataManager::getInstance()->getAchievementData(_id);
}

int AchievementData::getProgress() const
{
    return _progress;
}

int AchievementData::getStatus() const
{
    return _status;
}