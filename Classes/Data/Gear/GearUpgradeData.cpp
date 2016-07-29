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
#include "ObjectBriefData.h"

using namespace std;

GearUpgradeData::GearUpgradeData(tinyxml2::XMLElement *xmlElement)
:AbstractUpgradeData(xmlElement)
{
    if (xmlElement)
    {
        {
            const char *data = xmlElement->Attribute("worth");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (auto iter = result.begin(); iter != result.end(); ++iter)
                {
                    ObjectBriefData* reward = new (nothrow) ObjectBriefData(*iter);
                    _soldRewards.insert(make_pair(reward->getId(), reward));
                }
            }
        }
    }
}

GearUpgradeData::~GearUpgradeData()
{
    Utils::clearMap(_soldRewards);
}

const ObjectBriefData* GearUpgradeData::getSoldReward(int type) const
{
    if (_soldRewards.find(type) != _soldRewards.end()) {
        return _soldRewards.at(type);
    }
    
    return nullptr;
}
