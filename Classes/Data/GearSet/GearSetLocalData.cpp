//
//  GearSetLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "GearSetLocalData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "AttributeData.h"

using namespace std;

GearSetLocalData::GearSetLocalData(tinyxml2::XMLElement *xmlElement)
:AbstractLocalData(xmlElement)
{
    if (xmlElement) {
        {
            const char *data = xmlElement->Attribute("set1");
            if (data) {
                AttributeData* attr = new (nothrow) AttributeData(data);
                _attributes.insert(make_pair(1, attr));
            }
        }
        {
            const char *data = xmlElement->Attribute("set2");
            if (data) {
                AttributeData* attr = new (nothrow) AttributeData(data);
                _attributes.insert(make_pair(2, attr));
            }
        }
        {
            const char *data = xmlElement->Attribute("set3");
            if (data) {
                AttributeData* attr = new (nothrow) AttributeData(data);
                _attributes.insert(make_pair(3, attr));
            }
        }
        {
            const char *data = xmlElement->Attribute("set4");
            if (data) {
                AttributeData* attr = new (nothrow) AttributeData(data);
                _attributes.insert(make_pair(4, attr));
            }
        }
    }
}

GearSetLocalData::~GearSetLocalData()
{
    Utils::clearMap(_attributes);
}

const AttributeData* GearSetLocalData::getAttribute(int gearCount) const
{
    if (_attributes.find(gearCount) != _attributes.end()) {
        return _attributes.at(gearCount);
    }
    
    return nullptr;
}