//
//  FriendData.cpp
//  Underworld_Client
//
//  Created by burst on 16/1/22.
//
//

#include "FriendData.h"
#include "Global.h"
#include "Utils.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

FriendData::FriendData(const rapidjson::Value& jsonDict)
{
    _userId = DICTOOL->getIntValue_json(jsonDict, "uid", INVALID_VALUE);
    _iconId = DICTOOL->getIntValue_json(jsonDict, "icon", 1);
    if (DICTOOL->checkObjectExist_json(jsonDict, "name"))
    {
        _userName = DICTOOL->getStringValue_json(jsonDict, "name");
    }
    _userName = DICTOOL->getIntValue_json(jsonDict, "uid");
    _level = DICTOOL->getIntValue_json(jsonDict, "level", 1);
}

FriendData::~FriendData()
{
    
}

int FriendData::getUserId() const
{
    return _userId;
}

int FriendData::getIconId() const
{
    return _iconId;
}

const std::string& FriendData::getUserName() const
{
    return _userName;
}

int FriendData::getLevel() const
{
    return _level;
}