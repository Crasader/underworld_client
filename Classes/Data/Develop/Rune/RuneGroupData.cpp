//
//  RuneGroupData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneGroupData.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

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
