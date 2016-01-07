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
#include "AttributeData.h"

using namespace std;

HeroUpgradeData::HeroUpgradeData(tinyxml2::XMLElement *xmlElement)
:AbstractUpgradeData(xmlElement)
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
    }
}

HeroUpgradeData::~HeroUpgradeData()
{
    
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
