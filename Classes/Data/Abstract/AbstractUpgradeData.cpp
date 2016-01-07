//
//  AbstractUpgradeData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AbstractUpgradeData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ResourceData.h"

using namespace std;

AbstractUpgradeData::AbstractUpgradeData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
,_unlockedLevel(0)
{
    if (xmlElement)
    {
        _id = atoi(xmlElement->Attribute("id"));
        _level = atoi(xmlElement->Attribute("level"));
        
        {
            const char *data = xmlElement->Attribute("unlock");
            if (data) {
                _unlockedLevel = atoi(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("worth");
            if (data)
            {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
                {
                    ResourceData* data = new (nothrow) ResourceData(*iter);
                    _cost.insert(make_pair(data->getId(), data));
                }
            }
        }
    }
}

AbstractUpgradeData::~AbstractUpgradeData()
{
    Utils::clearMap(_cost);
}

int AbstractUpgradeData::getId() const
{
    return _id;
}

int AbstractUpgradeData::getLevel() const
{
    return _level;
}

int AbstractUpgradeData::getUnlockedLevel() const
{
    return _unlockedLevel;
}

int AbstractUpgradeData::getResourceCount(ResourceType type) const
{
    if (_cost.find(type) != _cost.end())
    {
        return _cost.at(type)->getCount();
    }
    
    return 99999;
}
