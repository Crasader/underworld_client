//
//  UserSimpleData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UserSimpleData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "CocosUtils.h"
#include "cocostudio/CocoStudio.h"
#include "CardSimpleData.h"

using namespace std;
using namespace cocostudio;

UserSimpleData::UserSimpleData(const rapidjson::Value& jsonDict)
:_uid(0)
,_icon(0)
,_level(0)
,_exp(0)
,_trophy(0)
,_guildIdx(0)
{
    
}

UserSimpleData::~UserSimpleData()
{
    for (auto data : _cards) {
        CC_SAFE_DELETE(data);
    }
}

int UserSimpleData::getUid() const
{
    return _uid;
}

const string& UserSimpleData::getUser() const
{
    return _user;
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

int UserSimpleData::getTrophy() const
{
    return _trophy;
}

int UserSimpleData::getGuildIdx() const
{
    return _guildIdx;
}

const string& UserSimpleData::getGuildName() const
{
    return _guildName;
}

const vector<CardSimpleData*>& UserSimpleData::getCards() const
{
    return _cards;
}
