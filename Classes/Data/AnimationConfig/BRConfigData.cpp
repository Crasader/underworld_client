//
//  BRConfigData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/4/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BRConfigData.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

BRConfigData::BRConfigData(tinyxml2::XMLElement *xmlElement)
{
    if (xmlElement) {
        _name = xmlElement->Attribute("render_key");
        {
            const char *data = xmlElement->Attribute("resource");
            if (data && strlen(data) > 0) {
                _resource = data;
            }
        }
    }
}

BRConfigData::~BRConfigData()
{
    
}

const string& BRConfigData::getName() const
{
    return _name;
}

const string& BRConfigData::getResource() const
{
    return _resource;
}
