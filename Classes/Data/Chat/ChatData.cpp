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
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

#pragma mark =====================================================
#pragma mark Chat Data
#pragma mark =====================================================

ChatData::ChatData(ChatType type, const rapidjson::Value& jsonDict)
:_id(0)
,_type(kChatType_World)
,_fromUserId(0)
,_toUserId(0)
,_timeStamp(0)
{
    this->_type = type;
    this->_fromUserId = DICTOOL->getIntValue_json(jsonDict, "from");
    this->_toUserId = DICTOOL->getIntValue_json(jsonDict, "to");
    if(DICTOOL->checkObjectExist_json(jsonDict, "fromName"))
    {
        this->_fromUserName = DICTOOL->getStringValue_json(jsonDict, "fromName");
    }
    if(DICTOOL->checkObjectExist_json(jsonDict, "toName"))
    {
        this->_toUserName = DICTOOL->getStringValue_json(jsonDict, "toName");
    }
    if(DICTOOL->checkObjectExist_json(jsonDict, "content"))
    {
        this->_message = DICTOOL->getStringValue_json(jsonDict, "content");
    }
    this->_timeStamp = DICTOOL->getIntValue_json(jsonDict, "time");
}

ChatData::~ChatData()
{
    
}

int ChatData::getId() const
{
    return _id;
}

ChatType ChatData::getType() const
{
    return _type;
}

long ChatData::getTimeStamp() const
{
    return _timeStamp;
}

int ChatData::getFromUserId() const
{
    return _fromUserId;
}

int ChatData::getToUserId() const
{
    return _toUserId;
}

const string& ChatData::getFromUserName() const
{
    return _fromUserName;
}

const string& ChatData::getToUserName() const
{
    return _toUserName;
}

const string& ChatData::getMessage() const
{
    return _message;
}