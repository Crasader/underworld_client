//
//  TowerUpgradeData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "TowerUpgradeData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ResourceData.h"

using namespace std;

TowerUpgradeData::TowerUpgradeData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
{
    if (xmlElement)
    {
        _id = atoi(xmlElement->Attribute("id"));
        _level = atoi(xmlElement->Attribute("level"));
        
        {
            const char *data = xmlElement->Attribute("cost");
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

TowerUpgradeData::~TowerUpgradeData()
{
    Utils::clearMap(_cost);
}

int TowerUpgradeData::getId() const
{
    return _id;
}

int TowerUpgradeData::level() const
{
    return _level;
}

int TowerUpgradeData::getResourceCount(ResourceType type) const
{
    if (_cost.find(type) != _cost.end())
    {
        return _cost.at(type)->getCount();
    }
    
    return 99999;
}
