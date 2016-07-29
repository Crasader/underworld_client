//
//  GearLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ObjectBriefData.h"

using namespace std;

GearLocalData::GearLocalData(tinyxml2::XMLElement *xmlElement)
:AbstractLocalData(xmlElement)
,_setId(0)
,_type(static_cast<GearType>(-1))
,_quality(static_cast<GearQuality>(-1))
,_maxCount(0)
{
    if (xmlElement) {
        _type = static_cast<GearType>('1' - '0');
        {
            const char *data = xmlElement->Attribute("max");
            if (data) {
                _maxCount = atoi(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("quality");
            if (data) {
                _quality = static_cast<GearQuality>(atoi(data));
            }
        }
        {
            const char *data = xmlElement->Attribute("attr1");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (int i = 0; i < result.size(); ++i)
                {
                    _attributes.insert(atoi(result.at(i).c_str()));
                }
            }
        }
        {
            const char *data = xmlElement->Attribute("attr2");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (int i = 0; i < result.size(); ++i)
                {
                    _subAttributes.insert(atoi(result.at(i).c_str()));
                }
            }
        }
    }
}

GearLocalData::~GearLocalData()
{
    
}

int GearLocalData::getSetId() const
{
    return _setId;
}

GearType GearLocalData::getType() const
{
    return _type;
}

GearQuality GearLocalData::getQuality() const
{
    return _quality;
}

int GearLocalData::getMaxCount() const
{
    return _maxCount;
}

const std::unordered_set<int>& GearLocalData::getAttributes() const
{
    return _attributes;
}

const std::unordered_set<int>& GearLocalData::getSubAttributes() const
{
    return _subAttributes;
}