//
//  GuildData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/5.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildData.h"
#include "JSonUtils.h"

GuildData::GuildData(const rapidjson::Value& jsonDict)
:_id(0)
,_type(Type::Open)
,_badge(0)
,_membersAmount(0)
,_totalTrophies(0)
,_requiredTrophies(0)
,_redEnvelopes(0)
,_donations(0)
{
    update(jsonDict);
}

GuildData::~GuildData() {}

void GuildData::update(const rapidjson::Value& jsonDict)
{
    JSonUtils::parse(_id, jsonDict, "id");
    JSonUtils::parse(_type, jsonDict, "jointype");
    JSonUtils::parse(_badge, jsonDict, "icon");
    JSonUtils::parse(_name, jsonDict, "name");
    JSonUtils::parse(_description, jsonDict, "notice");
    JSonUtils::parse(_membersAmount, jsonDict, "member");
    JSonUtils::parse(_totalTrophies, jsonDict, "trophy");
    JSonUtils::parse(_requiredTrophies, jsonDict, "jointrophy");
    JSonUtils::parse(_redEnvelopes, jsonDict, "redpacket");
    JSonUtils::parse(_donations, jsonDict, "donate");
}

int GuildData::getId() const
{
    return _id;
}

GuildData::Type GuildData::getType() const
{
    return _type;
}

int GuildData::getBadge() const
{
    return _badge;
}

const std::string& GuildData::getName() const
{
    return _name;
}

const std::string& GuildData::getDescription() const
{
    return _description;
}

int GuildData::getMembersAmount() const
{
    return _membersAmount;
}

int GuildData::getTotalTrophies() const
{
    return _totalTrophies;
}

int GuildData::getRequiredTrophies() const
{
    return _requiredTrophies;
}

int GuildData::getRedEnvelopes() const
{
    return _redEnvelopes;
}

int GuildData::getDonations() const
{
    return _donations;
}
