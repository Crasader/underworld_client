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

using namespace std;

GearUpgradeData::GearUpgradeData(tinyxml2::XMLElement *xmlElement)
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

GearUpgradeData::~GearUpgradeData()
{
    Utils::clearMap(_cost);
}

int GearUpgradeData::getId() const
{
    return _id;
}

int GearUpgradeData::level() const
{
    return _level;
}

int GearUpgradeData::getResourceCount(ResourceType type) const
{
    if (_cost.find(type) != _cost.end())
    {
        return _cost.at(type)->getCount();
    }
    
    return 99999;
}
