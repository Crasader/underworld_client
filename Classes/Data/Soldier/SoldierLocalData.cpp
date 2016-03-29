//
//  SoldierLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "SoldierLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "AttributeData.h"

using namespace std;

SoldierLocalData::SoldierLocalData(tinyxml2::XMLElement *xmlElement)
:AbstractLocalData(xmlElement)
{
    if (xmlElement) {
        {
            const char *data = xmlElement->Attribute("attr");
            if (data) {
                vector<string> result;
                Utils::split(result, data, ",", "");
                for (auto iter = result.begin(); iter != result.end(); ++iter)
                {
                    AttributeData* attr = new (nothrow) AttributeData(*iter);
                    _attributes.insert(make_pair(attr->getId(), attr));
                }
            }
        }
    }
}

SoldierLocalData::~SoldierLocalData()
{
    Utils::clearMap(_attributes);
}

const unordered_map<int, AttributeData *>& SoldierLocalData::getAttributes() const
{
    return _attributes;
}

const AttributeData* SoldierLocalData::getAttribute(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}
