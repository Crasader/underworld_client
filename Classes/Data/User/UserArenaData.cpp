//
//  UserArenaData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/6.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UserArenaData.h"
#include "JSonUtils.h"

UserArenaData::UserArenaData(const rapidjson::Value& jsonDict)
:_uid(0)
,_trophy(0)
{
    update(jsonDict);
}

UserArenaData::~UserArenaData() {}

void UserArenaData::update(const rapidjson::Value& jsonDict)
{
    JSonUtils::parse(_uid, jsonDict, "uid");
    JSonUtils::parse(_trophy, jsonDict, "trophy");
}

int UserArenaData::getUid() const
{
    return _uid;
}

int UserArenaData::getTrophy() const
{
    return _trophy;
}
