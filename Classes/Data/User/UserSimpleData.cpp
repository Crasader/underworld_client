//
//  UserSimpleData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "UserSimpleData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

#pragma mark =====================================================
#pragma mark User Simple Data
#pragma mark =====================================================

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
