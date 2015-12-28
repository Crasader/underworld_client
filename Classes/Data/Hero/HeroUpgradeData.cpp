//
//  HeroUpgradeData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "HeroUpgradeData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ResourceData.h"

using namespace std;

HeroUpgradeData::HeroUpgradeData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
{
    if (xmlElement)
    {
        _id = atoi(xmlElement->Attribute("id"));
        _level = atoi(xmlElement->Attribute("level"));
        
        const char *cost = xmlElement->Attribute("cost");
        if (cost)
        {
            vector<string> result;
            Utils::split(result, cost, ";", "");
            for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
            {
                ResourceData* data = new ResourceData(*iter);
                _cost.insert(make_pair(data->getId(), data));
            }
        }
    }
}

HeroUpgradeData::~HeroUpgradeData()
{
    Utils::clearMap(_cost);
}

int HeroUpgradeData::getId() const
{
    return _id;
}

int HeroUpgradeData::level() const
{
    return _level;
}

int HeroUpgradeData::getResourceCount(ResourceType type) const
{
    if (_cost.find(type) != _cost.end())
    {
        return _cost.at(type)->getCount();
    }
    
    return 99999;
}
