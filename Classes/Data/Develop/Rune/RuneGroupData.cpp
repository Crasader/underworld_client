//
//  RuneGroupData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneGroupData.h"
#include "JSonUtils.h"

using namespace std;

RuneGroupData::RuneGroupData(const rapidjson::Value& jsonDict)
:RuneData(jsonDict)
{
    update(jsonDict);
}

RuneGroupData::~RuneGroupData() {}

void RuneGroupData::update(const rapidjson::Value& jsonDict)
{
    RuneData::update(jsonDict);
}
