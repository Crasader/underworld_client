//
//  QuestData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestData.h"
#include "DataManager.h"
#include "JSonUtils.h"
#include "QuestProperty.h"

QuestData::QuestData(const rapidjson::Value& jsonDict)
:_type(QuestType::Daily)
,_id(0)
,_progress(0)
,_status(0)
{
    JSonUtils::parse(_id, jsonDict, "qid");
    JSonUtils::parse(_progress, jsonDict, "progress");
    JSonUtils::parse(_status, jsonDict, "status");
}

QuestData::~QuestData()
{
    
}

QuestType QuestData::getType() const
{
    return _type;
}

int QuestData::getId() const
{
    return _id;
}

const QuestProperty* QuestData::getProperty() const
{
    return DataManager::getInstance()->getQuestProperty(_type, _id);
}

int QuestData::getProgress() const
{
    return _progress;
}

int QuestData::getStatus() const
{
    return _status;
}
