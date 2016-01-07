//
//  UserData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UserData.h"

using namespace std;

UserData::UserData(const rapidjson::Value& jsonDict)
:_userId(0)
{
    
}

UserData::~UserData()
{
    
}

int UserData::getUserId() const
{
    return _userId;
}
