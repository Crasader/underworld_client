//
//  UserSimpleData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UserSimpleData.h"

using namespace std;

UserSimpleData::UserSimpleData(const rapidjson::Value& jsonDict)
:_userId(0)
{
    
}

UserSimpleData::~UserSimpleData()
{
    
}

int UserSimpleData::getUserId() const
{
    return _userId;
}
