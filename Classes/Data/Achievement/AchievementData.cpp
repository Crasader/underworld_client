//
//  AchievementData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AchievementData.h"
#include "DataManager.h"
#include "JSonUtils.h"
#include "AchievementProperty.h"

AchievementData::AchievementData(const rapidjson::Value& jsonDict)
:_id(0)
,_progress(0)
,_status(0)
{
    JSonUtils::parse(_id, jsonDict, "qid");
    JSonUtils::parse(_progress, jsonDict, "progress");
    JSonUtils::parse(_status, jsonDict, "status");
}

AchievementData::~AchievementData()
{
    
}

int AchievementData::getId() const
{
    return _id;
}

const AchievementProperty* AchievementData::getProperty() const
{
    return DataManager::getInstance()->getAchievementProperty(_id);
}

int AchievementData::getProgress() const
{
    return _progress;
}

int AchievementData::getStatus() const
{
    return _status;
}
