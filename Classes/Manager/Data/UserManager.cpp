//
//  UserManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UserManager.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

UserManager::UserManager()
{
}

UserManager::~UserManager()
{
    
}

void UserManager::init(const rapidjson::Value& jsonDict)
{
    
}

const vector<UserSimpleData*>& UserManager::getFriends() const
{
    return _friends;
}

void UserManager::addFriend(int userId)
{
    
}

void UserManager::removeFriend(int userId)
{
    
}

const vector<UserSimpleData*>& UserManager::getBlackList() const
{
    return _blackList;
}

void UserManager::addToBlackList(int userId)
{
    
}

void UserManager::removeFromBlackList(int userId)
{
    
}
