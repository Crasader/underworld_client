//
//  BillboardData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BillboardData.h"
#include "JSonUtils.h"
#include "UserSimpleData.h"

BillboardData::BillboardData(const rapidjson::Value& jsonDict)
:_rank(0)
,_userData(nullptr)
{
    update(jsonDict);
}

BillboardData::~BillboardData()
{
    CC_SAFE_DELETE(_userData);
}

void BillboardData::update(const rapidjson::Value& jsonDict)
{
    JSonUtils::parse(_rank, jsonDict, "rank");
    if (JSonUtils::isExist(jsonDict, "user")) {
        const auto& value(DICTOOL->getSubDictionary_json(jsonDict, "user"));
        if (_userData) {
            _userData->update(value);
        } else {
            _userData = new (std::nothrow) UserSimpleData(value);
        }
    }
}

int BillboardData::getRank() const
{
    return _rank;
}

const UserSimpleData* BillboardData::getUserData() const
{
    return _userData;
}
