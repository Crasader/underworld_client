//
//  ChatData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatData.h"
#include "Utils.h"
#include "JSonUtils.h"
#include "ObjectBriefData.h"

using namespace std;

ChatData::ChatData(const rapidjson::Value& jsonDict)
:_id(0)
,_type(ChatType::None)
,_uid(0)
,_icon(0)
,_time(0)
{
    if (JSonUtils::isExist(jsonDict, "from")) {
        const auto& value = DICTOOL->getSubDictionary_json(jsonDict, "from");
        JSonUtils::parse(_uid, value, "id");
        JSonUtils::parse(_user, value, "name");
    }
    
    string timeString = JSonUtils::parse<string>(jsonDict, "time");
    _time = atol(timeString.c_str());
    _formattedTime = Utils::getTimeString(_time);
    JSonUtils::parse(_type, jsonDict, "channel");
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
