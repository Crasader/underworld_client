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
:_dbIdx(0)
,_idx(0)
,_level(0)
{
    _dbIdx = DICTOOL->getIntValue_json(jsonDict, "db");
    _idx = DICTOOL->getIntValue_json(jsonDict, "id");
    _level = DICTOOL->getIntValue_json(jsonDict, "level");
}

RuneData::~RuneData() {}

int RuneData::getDbIdx() const
{
    return _dbIdx;
}

int RuneData::getIdx() const
{
    return _idx;
}

int RuneData::getLevel() const
{
    return _level;
}
