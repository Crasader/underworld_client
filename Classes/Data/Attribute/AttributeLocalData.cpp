//
//  AttributeLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AttributeLocalData.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

AttributeLocalData::AttributeLocalData(tinyxml2::XMLElement *xmlElement)
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
    }
}

AttributeLocalData::~AttributeLocalData()
{
    
}

int AttributeLocalData::getId() const
{
    return _id;
}

const string& AttributeLocalData::getName() const
{
    return _name;
}
