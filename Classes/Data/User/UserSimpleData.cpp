//
//  UserSimpleData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UserSimpleData.h"
#include "Utils.h"
#include "JSonUtils.h"
#include "UserArenaData.h"
#include "UserGuildData.h"
#include "CardData.h"

UserSimpleData::UserSimpleData(const rapidjson::Value& jsonDict)
:_uid(0)
,_icon(0)
,_level(0)
,_exp(0)
,_arenaData(nullptr)
,_guildData(nullptr)
{
    update(jsonDict);
}

UserSimpleData::~UserSimpleData()
{
    CC_SAFE_DELETE(_arenaData);
    CC_SAFE_DELETE(_guildData);
    for (auto data : _cards) {
        CC_SAFE_DELETE(data);
    }
}

void UserSimpleData::update(const rapidjson::Value& jsonDict)
{
    JSonUtils::parse(_uid, jsonDict, "id");
    JSonUtils::parse(_name, jsonDict, "jointype");
    JSonUtils::parse(_icon, jsonDict, "icon");
    JSonUtils::parse(_level, jsonDict, "name");
    JSonUtils::parse(_exp, jsonDict, "notice");
    
    if (JSonUtils::isExist(jsonDict, "guildinfo")) {
        const auto& value(DICTOOL->getSubDictionary_json(jsonDict, "guildinfo"));
        if (_guildData) {
            _guildData->update(value);
        } else {
            _guildData = new (std::nothrow) UserGuildData(value);
        }
    }
    
    if (JSonUtils::isExist(jsonDict, "pvp")) {
        const auto& value(DICTOOL->getSubDictionary_json(jsonDict, "pvp"));
        if (_arenaData) {
            _arenaData->update(value);
        } else {
            _arenaData = new (std::nothrow) UserArenaData(value);
        }
    }
}

int UserSimpleData::getUid() const
{
    return _uid;
}

const std::string& UserSimpleData::getName() const
{
    return _name;
}

int UserSimpleData::getIcon() const
{
    return _icon;
}

int UserSimpleData::getLevel() const
{
    return _level;
}

int UserSimpleData::getExp() const
{
    return _exp;
}

const UserArenaData* UserSimpleData::getArenaData() const
{
    return _arenaData;
}

const UserGuildData* UserSimpleData::getGuildData() const
{
    return _guildData;
}

const std::vector<CardData*>& UserSimpleData::getCards() const
{
    return _cards;
}
