//
//  AbstractLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AbstractLocalData.h"
#include "RewardData.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "AttributeData.h"

using namespace std;

AbstractLocalData::AbstractLocalData(tinyxml2::XMLElement *xmlElement)
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

AbstractLocalData::~AbstractLocalData()
{
    
}

int AbstractLocalData::getId() const
{
    return _id;
}

const string& AbstractLocalData::getName() const
{
    return _name;
}

const string& AbstractLocalData::getDescription() const
{
    return _description;
}
