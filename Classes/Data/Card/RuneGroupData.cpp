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
,_amount(0)
{
    update(jsonDict);
}

RuneGroupData::~RuneGroupData() {}

void RuneGroupData::update(const rapidjson::Value& jsonDict)
{
    RuneData::update(jsonDict);
    _amount = DICTOOL->getIntValue_json(jsonDict, "amount");
}

int RuneGroupData::getAmount() const
{
    return _amount;
}
