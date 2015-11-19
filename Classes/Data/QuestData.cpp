//
//  QuestData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

#pragma mark =====================================================
#pragma mark Quest Reward
#pragma mark =====================================================

QuestReward::QuestReward(const string& content)
:_id(0)
,_count(0)
{
    vector<string> result;
    Utils::split(result, content, "_", "");
    if (result.size() > 1) {
        _id = atoi(result.at(0).c_str());
        _count = atoi(result.at(1).c_str());
    } else {
        assert(false);
    }
}

QuestReward::~QuestReward()
{
    
}

int QuestReward::getId() const
{
    return _id;
}

int QuestReward::getCount() const
{
    return _count;
}

#pragma mark =====================================================
#pragma mark Quest Data
#pragma mark =====================================================

QuestData::QuestData(tinyxml2::XMLElement *xmlElement)
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
                QuestReward* qr = new (nothrow) QuestReward(result.at(i));
                _rewards.push_back(qr);
            }
        }
        
        _description = xmlElement->Attribute("desc");
    }
}

QuestData::~QuestData()
{
    for (int i = 0; i < _rewards.size(); ++i) {
        CC_SAFE_DELETE(_rewards.at(i));
    }
}

int QuestData::getId() const
{
    return _id;
}

int QuestData::getRequiredCount() const
{
    return _requiredCount;
}

const vector<QuestReward *>& QuestData::getRewards() const
{
    return _rewards;
}

const string& QuestData::getDescription() const
{
    return _description;
}
