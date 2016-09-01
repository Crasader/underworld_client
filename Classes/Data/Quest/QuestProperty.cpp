//
//  QuestProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestProperty.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ConditionData.h"
#include "ContentData.h"
#include "ObjectBriefData.h"

using namespace std;

QuestProperty::QuestProperty(tinyxml2::XMLElement *xmlElement)
:AbstractProperty(xmlElement)
,_race(0)
{
    if (xmlElement) {
        attribute2Int(xmlElement, "race", _race);
        
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
            const char *data = xmlElement->Attribute("content");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
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
                Utils::split(result, data, ",", "");
                for (int i = 0; i < result.size(); ++i) {
                    ObjectBriefData* rd = new (nothrow) ObjectBriefData(result.at(i));
                    _rewards.push_back(rd);
                }
            }
        }
    }
}

QuestProperty::~QuestProperty()
{
    Utils::clearVector(_conditions);
    Utils::clearVector(_contents);
    Utils::clearVector(_rewards);
}

int QuestProperty::getRace() const
{
    return _race;
}

const vector<ConditionData*>& QuestProperty::getConditions() const
{
    return _conditions;
}

const vector<ContentData*>& QuestProperty::getContents() const
{
    return _contents;
}

const vector<ObjectBriefData *>& QuestProperty::getRewards() const
{
    return _rewards;
}
