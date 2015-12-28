//
//  HeroLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "HeroLocalData.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

HeroLocalData::HeroLocalData(tinyxml2::XMLElement *xmlElement)
:_id(0)
{
    if (xmlElement) {
        _id = atoi(xmlElement->Attribute("id"));
        
        {
            const char *data = xmlElement->Attribute("name");
            if (data) {
                _name.assign(data);
            }
        }
        {
            const char *data = xmlElement->Attribute("desc");
            if (data) {
                _description.assign(data);
            }
        }
    }
}

HeroLocalData::~HeroLocalData()
{
    
}

int HeroLocalData::getId() const
{
    return _id;
}

const string& HeroLocalData::getName() const
{
    return _name;
}

const string& HeroLocalData::getDescription() const
{
    return _description;
}
