//
//  SoldierQualityData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "SoldierQualityData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ResourceData.h"
#include "AttributeData.h"

using namespace std;

SoldierQualityData::SoldierQualityData(tinyxml2::XMLElement *xmlElement)
:AbstractUpgradeData(xmlElement)
,_output(0)
{
    if (xmlElement)
    {
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
        {
            const char *data = xmlElement->Attribute("output");
            if (data) {
                _output = atoi(data);
            }
        }
    }
}

SoldierQualityData::~SoldierQualityData()
{
    Utils::clearMap(_attributes);
}

const map<int, AttributeData *>& SoldierQualityData::getAttributes() const
{
    return _attributes;
}

const AttributeData* SoldierQualityData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}

int SoldierQualityData::getOutput() const
{
    return _output;
}
