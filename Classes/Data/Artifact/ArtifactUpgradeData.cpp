//
//  ArtifactUpgradeData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ArtifactUpgradeData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ResourceData.h"
#include "AttributeData.h"

using namespace std;

ArtifactUpgradeData::ArtifactUpgradeData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
{
    if (xmlElement)
    {
        _id = atoi(xmlElement->Attribute("id"));
        _level = atoi(xmlElement->Attribute("level"));
        
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

ArtifactUpgradeData::~ArtifactUpgradeData()
{
    Utils::clearMap(_cost);
    Utils::clearMap(_attributes);
}

int ArtifactUpgradeData::getId() const
{
    return _id;
}

int ArtifactUpgradeData::level() const
{
    return _level;
}

int ArtifactUpgradeData::getResourceCount(ResourceType type) const
{
    if (_cost.find(type) != _cost.end())
    {
        return _cost.at(type)->getCount();
    }
    
    return 99999;
}

const map<int, AttributeData *>& ArtifactUpgradeData::getAttributes() const
{
    return _attributes;
}

const AttributeData* ArtifactUpgradeData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}
