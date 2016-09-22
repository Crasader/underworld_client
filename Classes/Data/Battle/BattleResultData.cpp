//
//  BattleResultData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/21.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BattleResultData.h"
#include "JSonUtils.h"
#include "ObjectBriefData.h"

using namespace std;

BattleResultData::BattleResultData(const rapidjson::Value& jsonDict)
:_result(Result::DRAW)
,_trophy(0)
{
    JSonUtils::parse(_result, jsonDict, "result");
    JSonUtils::parse(_trophy, jsonDict, "trophy");
    
    if (JSonUtils::isExist(jsonDict, "reward")) {
        // TODO:
        const auto& value = DICTOOL->getSubDictionary_json(jsonDict, "reward");
        _rewards.push_back(new (nothrow) ObjectBriefData(""));
    }
}

BattleResultData::~BattleResultData()
{
    for (auto data : _rewards) {
        CC_SAFE_DELETE(data);
    }
}

BattleResultData::Result BattleResultData::getResult() const
{
    return _result;
}

int BattleResultData::getTrophy() const
{
    return _trophy;
}

const std::vector<ObjectBriefData*>& BattleResultData::getRewards() const
{
    return _rewards;
}
