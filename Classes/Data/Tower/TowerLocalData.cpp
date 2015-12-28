//
//  TowerLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "TowerLocalData.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

TowerLocalData::TowerLocalData(tinyxml2::XMLElement *xmlElement)
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

TowerLocalData::~TowerLocalData()
{
    
}

int TowerLocalData::getId() const
{
    return _id;
}

const string& TowerLocalData::getName() const
{
    return _name;
}

const string& TowerLocalData::getDescription() const
{
    return _description;
}
