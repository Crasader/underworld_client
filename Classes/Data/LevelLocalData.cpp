//
//  LevelLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "LevelLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ConditionData.h"
#include "RewardData.h"

using namespace std;

LevelLocalData::LevelLocalData(tinyxml2::XMLElement *xmlElement)
:_levelId(0)
{
    if (xmlElement) {
        _levelId = atoi(xmlElement->Attribute("id"));
        {
            const char *data = xmlElement->Attribute("cond");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ";", "");
                for (int i = 0; i < result.size(); ++i) {
                    ConditionData* cd = new (nothrow) ConditionData(result.at(i));
                    _conditions.push_back(cd);
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("reward");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ";", "");
                for (int i = 0; i < result.size(); ++i) {
                    RewardData* rd = new (nothrow) RewardData(result.at(i));
                    _rewards.push_back(rd);
                }
            }
        }
    }
}

LevelLocalData::~LevelLocalData()
{
    for (int i = 0; i < _conditions.size(); ++i) {
        CC_SAFE_DELETE(_conditions.at(i));
    }
    for (int i = 0; i < _rewards.size(); ++i) {
        CC_SAFE_DELETE(_rewards.at(i));
    }
}

int LevelLocalData::getLevelId() const
{
    return _levelId;
}

const vector<ConditionData*>& LevelLocalData::getConditions() const
{
    return _conditions;
}

const vector<RewardData *>& LevelLocalData::getRewards() const
{
    return _rewards;
}