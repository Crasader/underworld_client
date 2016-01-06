//
//  GearUpgradeData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearUpgradeData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ResourceData.h"
#include "RewardData.h"

using namespace std;

GearUpgradeData::GearUpgradeData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
,_unlockedLevel(0)
{
    if (xmlElement)
    {
        _id = atoi(xmlElement->Attribute("id"));
        _level = atoi(xmlElement->Attribute("level"));
        _unlockedLevel = atoi(xmlElement->Attribute("unlock"));
        
        {
            const char *data = xmlElement->Attribute("cost");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
                {
                    ResourceData* data = new (nothrow) ResourceData(*iter);
                    _cost.insert(make_pair(data->getId(), data));
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("worth");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ";", "");
                for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
                {
                    RewardData* reward = new (nothrow) RewardData(*iter);
                    _soldRewards.push_back(reward);
                }
            }
        }
    }
}

GearUpgradeData::~GearUpgradeData()
{
    Utils::clearMap(_cost);
    Utils::clearVector(_soldRewards);
}

int GearUpgradeData::getId() const
{
    return _id;
}

int GearUpgradeData::level() const
{
    return _level;
}

int GearUpgradeData::getUnlockedLevel() const
{
    return _unlockedLevel;
}

int GearUpgradeData::getResourceCount(ResourceType type) const
{
    if (_cost.find(type) != _cost.end())
    {
        return _cost.at(type)->getCount();
    }
    
    return 99999;
}

const vector<RewardData*>& GearUpgradeData::getSoldRewards() const
{
    return _soldRewards;
}
