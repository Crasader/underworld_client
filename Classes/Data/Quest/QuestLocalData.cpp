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
#include "ConditionData.h"
#include "ContentData.h"
#include "RewardData.h"

using namespace std;

QuestLocalData::QuestLocalData(tinyxml2::XMLElement *xmlElement)
:_id(0)
{
    if (xmlElement) {
        _id = atoi(xmlElement->Attribute("id"));
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
            const char *data = xmlElement->Attribute("content");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ";", "");
                for (int i = 0; i < result.size(); ++i) {
                    ContentData* cd = new (nothrow) ContentData(result.at(i));
                    _contents.push_back(cd);
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
        {
            const char *data = xmlElement->Attribute("taskname");
            if (data) {
                _name.assign(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("desc");
            if (data) {
                _description.assign(data);
            }
        }
    }
}

QuestLocalData::~QuestLocalData()
{
    for (int i = 0; i < _conditions.size(); ++i) {
        CC_SAFE_DELETE(_conditions.at(i));
    }
    for (int i = 0; i < _contents.size(); ++i) {
        CC_SAFE_DELETE(_contents.at(i));
    }
    for (int i = 0; i < _rewards.size(); ++i) {
        CC_SAFE_DELETE(_rewards.at(i));
    }
}

int QuestLocalData::getId() const
{
    return _id;
}

const vector<ConditionData*>& QuestLocalData::getConditions() const
{
    return _conditions;
}

const vector<ContentData*>& QuestLocalData::getContents() const
{
    return _contents;
}

const vector<RewardData *>& QuestLocalData::getRewards() const
{
    return _rewards;
}

const string& QuestLocalData::getName() const
{
    return _name;
}

const string& QuestLocalData::getDescription() const
{
    return _description;
}
