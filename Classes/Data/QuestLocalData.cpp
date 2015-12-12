//
//  QuestLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "RewardData.h"

using namespace std;

QuestLocalData::QuestLocalData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_requiredCount(0)
{
    if (xmlElement) {
        const char* content = xmlElement->Attribute("content");
        if (content) {
            vector<string> result;
            Utils::split(result, content, "_", "");
            if (result.size() > 2) {
                _requiredCount = atoi(result.at(1).c_str());
            } else {
                assert(false);
            }
        }
        
        const char* reward = xmlElement->Attribute("reward");
        if (reward) {
            vector<string> result;
            Utils::split(result, content, ";", "");
            for (int i = 0; i < result.size(); ++i) {
                RewardData* qr = new (nothrow) RewardData(result.at(i));
                _rewards.push_back(qr);
            }
        }
        
        _description = xmlElement->Attribute("desc");
    }
}

QuestLocalData::~QuestLocalData()
{
    for (int i = 0; i < _rewards.size(); ++i) {
        CC_SAFE_DELETE(_rewards.at(i));
    }
}

int QuestLocalData::getId() const
{
    return _id;
}

int QuestLocalData::getRequiredCount() const
{
    return _requiredCount;
}

const vector<RewardData *>& QuestLocalData::getRewards() const
{
    return _rewards;
}

const string& QuestLocalData::getDescription() const
{
    return _description;
}
