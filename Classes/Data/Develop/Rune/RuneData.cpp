//
//  RuneData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneData.h"
#include "JSonUtils.h"

using namespace std;

RuneData::RuneData(const rapidjson::Value& jsonDict)
:AbstractData(jsonDict)
{
    update(jsonDict);
}

RuneData::~RuneData() {}

void RuneData::update(const rapidjson::Value& jsonDict)
{
    AbstractData::update(jsonDict);
}
