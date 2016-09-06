//
//  ChatData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatData.h"
#include "Utils.h"
#include "CocosUtils.h"
#include "JSonUtils.h"
#include "ObjectBriefData.h"

using namespace std;

ChatData::ChatData(ChatType type, const rapidjson::Value& jsonDict)
:_id(0)
,_type(type)
,_uid(0)
,_icon(0)
,_time(0)
{
    JSonUtils::parse(_uid, jsonDict, "uid");
    JSonUtils::parse(_user, jsonDict, "user");
    JSonUtils::parse(_time, jsonDict, "time");
    _formattedTime = CocosUtils::getFormattedTime(_time);
    JSonUtils::parse(_message, jsonDict, "content");
    
    string rewards;
    JSonUtils::parse(rewards, jsonDict, "rewards");
    vector<string> result;
    Utils::split(result, rewards, ";", "");
    for (int i = 0; i < result.size(); ++i) {
        auto data = new ObjectBriefData(result.at(i));
        _rewards.push_back(data);
    }
}

ChatData::~ChatData()
{
    for (auto data : _rewards) {
        CC_SAFE_DELETE(data);
    }
}

int ChatData::getId() const
{
    return _id;
}

ChatType ChatData::getType() const
{
    return _type;
}

int ChatData::getUid() const
{
    return _uid;
}

int ChatData::getIcon() const
{
    return _icon;
}

const string& ChatData::getUser() const
{
    return _user;
}

long ChatData::getTime() const
{
    return _time;
}

const string& ChatData::getFormattedTime() const
{
    return _formattedTime;
}

const string& ChatData::getMessage() const
{
    return _message;
}

const vector<ObjectBriefData*>& ChatData::getRewards() const
{
    return _rewards;
}
