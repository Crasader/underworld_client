//
//  UserGuildData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/6.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UserGuildData.h"
#include "JSonUtils.h"
#include "GuildData.h"

UserGuildData::UserGuildData(const rapidjson::Value& jsonDict)
:_uid(0)
,_guildData(nullptr)
,_role(Role::None)
,_countdown(0)
{
    update(jsonDict);
}

UserGuildData::~UserGuildData()
{
    CC_SAFE_DELETE(_guildData);
}

void UserGuildData::update(const rapidjson::Value& jsonDict)
{
    JSonUtils::parse(_uid, jsonDict, "uid");
    JSonUtils::parse(_role, jsonDict, "position");
    JSonUtils::parse(_countdown, jsonDict, "cardrequire");
    
    if (JSonUtils::isExist(jsonDict, "guild")) {
        const auto& value(DICTOOL->getSubDictionary_json(jsonDict, "guild"));
        if (_guildData) {
            _guildData->update(value);
        } else {
            _guildData = new (std::nothrow) GuildData(value);
        }
    }
}

int UserGuildData::getUid() const
{
    return _uid;
}

const GuildData* UserGuildData::getGuildData() const
{
    return _guildData;
}

UserGuildData::Role UserGuildData::getRole() const
{
    return _role;
}

int UserGuildData::getDonateCountdown() const
{
    return _countdown;
}
