//
//  GearAttributeData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearAttributeData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

using namespace std;

GearAttributeData::GearAttributeData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
,_attributeId(0)
{
    if (xmlElement)
    {
        _id = atoi(xmlElement->Attribute("id"));
        _level = atoi(xmlElement->Attribute("level"));
        _attributeId = atoi(xmlElement->Attribute("key"));
        
        const char *value = xmlElement->Attribute("value");
        if (value)
        {
            vector<string> result;
            Utils::split(result, value, ",", "");
            for (int i = 0; i < result.size(); ++i)
            {
                _attributes.insert(make_pair(static_cast<GearQuality>(i), atoi(result.at(i).c_str())));
            }
        }
    }
}

GearAttributeData::~GearAttributeData()
{
    
}

int GearAttributeData::getId() const
{
    return _id;
}

int GearAttributeData::level() const
{
    return _level;
}

int GearAttributeData::getAttributeId() const
{
    return _attributeId;
}

int GearAttributeData::getAttributeValue(GearQuality type) const
{
    if (_attributes.find(type) != _attributes.end()) {
        return _attributes.at(type);
    }
    
    return 0;
}
