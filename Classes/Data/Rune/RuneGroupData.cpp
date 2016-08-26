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
    _amount = DICTOOL->getIntValue_json(jsonDict, "amount");
}

RuneGroupData::~RuneGroupData() {}

int RuneGroupData::getAmount() const
{
    return _amount;
}
