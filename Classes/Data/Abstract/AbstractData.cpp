//
//  AbstractData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AbstractData.h"
#include "JSonUtils.h"
#include "DataManager.h"

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
    JSonUtils::parse(_dbId, jsonDict, "db");
    JSonUtils::parse(_id, jsonDict, "id");
    JSonUtils::parse(_level, jsonDict, "level");
    JSonUtils::parse(_amount, jsonDict, "amount");
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
