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

using namespace std;

#pragma mark =====================================================
#pragma mark Quest Data
#pragma mark =====================================================

ChatData::ChatData(ChatType type, tinyxml2::XMLElement *xmlElement)
:_id(0)
,_type(kChatType_World)
,_userId(0)
,_timeStamp(0)
{
    if (xmlElement) {
        
    }
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

int ChatData::getUserId() const
{
    return _userId;
}

const string& ChatData::getMessage() const
{
    return _message;
}