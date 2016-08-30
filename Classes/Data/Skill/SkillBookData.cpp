//
//  SkillBookData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SkillBookData.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

SkillBookData::SkillBookData(const rapidjson::Value& jsonDict)
:_id(0)
,_amount(0)
{
    _id = DICTOOL->getIntValue_json(jsonDict, "id");
    _amount = DICTOOL->getIntValue_json(jsonDict, "amount");
}

SkillBookData::~SkillBookData() {}

int SkillBookData::getId() const
{
    return _id;
}

int SkillBookData::getAmount() const
{
    return _amount;
}
