//
//  AbstractData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AbstractData.h"
#include "cocostudio/CocoStudio.h"
#include "DataManager.h"
#include "ObjectUtils.h"

using namespace cocostudio;

int AbstractData::getCardId(const AbstractData* data)
{
    return data ? data->getId() : 0;
}

AbstractData::AbstractData(const rapidjson::Value& jsonDict)
:_dbId(0)
,_id(0)
,_level(0)
,_amount(0)
{
    update(jsonDict);
}

AbstractData::~AbstractData() {}

void AbstractData::update(const rapidjson::Value& jsonDict)
{
    json2Int(jsonDict, "db", _dbId);
    json2Int(jsonDict, "id", _id);
    json2Int(jsonDict, "level", _level);
    json2Int(jsonDict, "amount", _amount);
}

int AbstractData::getDbId() const
{
    return _dbId;
}

int AbstractData::getId() const
{
    return _id;
}

int AbstractData::getLevel() const
{
    return _level;
}

int AbstractData::getAmount() const
{
    return _amount;
}

const AbstractProperty* AbstractData::getProperty() const
{
    return DataManager::getInstance()->getProperty(_id);
}

const AbstractUpgradeProperty* AbstractData::getUpgradeProperty() const
{
    return DataManager::getInstance()->getUpgradeProperty(_id, _level);
}

const AbstractUpgradeProperty* AbstractData::getNextLevelUpgradeProperty() const
{
    return DataManager::getInstance()->getUpgradeProperty(_id, _level + 1);
}

void AbstractData::json2Int(const rapidjson::Value& jsonDict, const char* key, int& output)
{
    if (DICTOOL->checkObjectExist_json(jsonDict, key)) {
        output = DICTOOL->getIntValue_json(jsonDict, key);
    }
}
