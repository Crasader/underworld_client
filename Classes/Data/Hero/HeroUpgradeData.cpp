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
#include "AttributeData.h"

using namespace std;

HeroUpgradeData::HeroUpgradeData(tinyxml2::XMLElement *xmlElement)
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
            const char *data = xmlElement->Attribute("resource");
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
        {
            const char *data = xmlElement->Attribute("attr");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (vector<string>::const_iterator iter = result.begin(); iter != result.end(); ++iter)
                {
                    AttributeData* attr = new (nothrow) AttributeData(*iter);
                    _attributes.insert(make_pair(attr->getId(), attr));
                }
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

int HeroUpgradeData::getUnlockedLevel() const
{
    return _unlockedLevel;
}

int HeroUpgradeData::getResourceCount(ResourceType type) const
{
    if (_cost.find(type) != _cost.end())
    {
        return _cost.at(type)->getCount();
    }
    
    return 99999;
}

const AttributeData* HeroUpgradeData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}

const map<int, AttributeData *>& HeroUpgradeData::getAttributes() const
{
    return _attributes;
}
