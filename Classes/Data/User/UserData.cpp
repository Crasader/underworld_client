//
//  UserData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UserData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

#pragma mark =====================================================
#pragma mark User Data
#pragma mark =====================================================

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
