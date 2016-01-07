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
:AbstractLocalData(xmlElement)
{
    if (xmlElement) {
        {
            const char *data = xmlElement->Attribute("cond");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
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
                Utils::split(result, data, ",", "");
                for (int i = 0; i < result.size(); ++i) {
                    RewardData* reward = new (nothrow) RewardData(result.at(i));
                    _rewards.insert(make_pair(reward->getId(), reward));
                }
            }
        }
    }
}

LevelLocalData::~LevelLocalData()
{
    Utils::clearVector(_conditions);
    Utils::clearMap(_rewards);
}

const vector<ConditionData*>& LevelLocalData::getConditions() const
{
    return _conditions;
}

const RewardData* LevelLocalData::getReward(int type) const
{
    if (_rewards.find(type) != _rewards.end()) {
        return _rewards.at(type);
    }
    
    return nullptr;
}