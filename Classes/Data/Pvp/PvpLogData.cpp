//
//  PvpLogData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "PvpLogData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

PvpLogData::PvpLogData(const rapidjson::Value& jsonDict)
:_homeUid(0)
,_awayUid(0)
{
    _homeUid = DICTOOL->getIntValue_json(jsonDict, "uid");
    _homeUser = DICTOOL->getStringValue_json(jsonDict, "user");
    _awayUid = DICTOOL->getIntValue_json(jsonDict, "uid");
    _awayUser = DICTOOL->getStringValue_json(jsonDict, "user");
}

PvpLogData::~PvpLogData()
{
    
}

const string& PvpLogData::getHomeUser() const
{
    return _homeUser;
}

const string& PvpLogData::getAwayUser() const
{
    return _awayUser;
}
