//
//  ChatData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ChatData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "CocosUtils.h"
#include "cocostudio/CocoStudio.h"
#include "ObjectBriefData.h"

using namespace std;
using namespace cocostudio;

ChatData::ChatData(ChatType type, const rapidjson::Value& jsonDict)
:_id(0)
,_type(type)
,_uid(0)
,_time(0)
{
    _uid = DICTOOL->getIntValue_json(jsonDict, "uid");
    _user = DICTOOL->getStringValue_json(jsonDict, "user");
    
    if (DICTOOL->checkObjectExist_json(jsonDict, "time")) {
        _time = DICTOOL->getIntValue_json(jsonDict, "time");
        _formattedTime = CocosUtils::getFormattedTime(_time);
    }
    
    if (DICTOOL->checkObjectExist_json(jsonDict, "content")) {
        _message = DICTOOL->getStringValue_json(jsonDict, "content");
    }
    
    if (DICTOOL->checkObjectExist_json(jsonDict, "rewards")) {
        auto s = DICTOOL->getStringValue_json(jsonDict, "rewards");
        vector<string> result;
        Utils::split(result, s, ";", "");
        for (int i = 0; i < result.size(); ++i) {
            auto data = new ObjectBriefData(result.at(i));
            _rewards.push_back(data);
        }
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
