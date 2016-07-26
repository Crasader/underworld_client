//
//  NoticeData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/25.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "NoticeData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

NoticeData::NoticeData(const rapidjson::Value& jsonDict)
:_id(0)
,_timeStamp(0)
{
    if(DICTOOL->checkObjectExist_json(jsonDict, "content")) {
        _message = DICTOOL->getStringValue_json(jsonDict, "content");
    }
    
    _timeStamp = DICTOOL->getIntValue_json(jsonDict, "time");
}

NoticeData::~NoticeData()
{
    
}

int NoticeData::getId() const
{
    return _id;
}

long NoticeData::getTimeStamp() const
{
    return _timeStamp;
}

const string& NoticeData::getMessage() const
{
    return _message;
}
