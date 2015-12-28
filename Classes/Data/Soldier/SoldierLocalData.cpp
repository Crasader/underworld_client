//
//  SoldierLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "SoldierLocalData.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

SoldierLocalData::SoldierLocalData(tinyxml2::XMLElement *xmlElement)
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

SoldierLocalData::~SoldierLocalData()
{
    
}

int SoldierLocalData::getId() const
{
    return _id;
}

const string& SoldierLocalData::getName() const
{
    return _name;
}

const string& SoldierLocalData::getDescription() const
{
    return _description;
}
