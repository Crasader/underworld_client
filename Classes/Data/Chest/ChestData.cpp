//
//  ChestData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "ChestData.h"
#include "JSonUtils.h"

ChestData::ChestData(const rapidjson::Value& jsonDict)
{
    update(jsonDict);
}

ChestData::~ChestData() {}

void ChestData::update(const rapidjson::Value& jsonDict)
{
    
}
