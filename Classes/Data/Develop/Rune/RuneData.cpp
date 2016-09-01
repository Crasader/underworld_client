//
//  RuneData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneData.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

RuneData::RuneData(const rapidjson::Value& jsonDict)
:_dbId(0)
,_id(0)
,_level(0)
{
    update(jsonDict);
}

RuneData::~RuneData() {}

void RuneData::update(const rapidjson::Value& jsonDict)
{
    _dbId = DICTOOL->getIntValue_json(jsonDict, "db");
    _id = DICTOOL->getIntValue_json(jsonDict, "id");
    _level = DICTOOL->getIntValue_json(jsonDict, "level");
}

int RuneData::getDbId() const
{
    return _dbId;
}

int RuneData::getId() const
{
    return _id;
}

int RuneData::getLevel() const
{
    return _level;
}

static const string empty("");

const string& RuneData::getName() const
{
    return empty;
}

const string& RuneData::getDescription() const
{
    return empty;
}

const string& RuneData::getEffect() const
{
    return empty;
}
