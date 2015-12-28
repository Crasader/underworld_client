//
//  SkillLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "SkillLocalData.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

SkillLocalData::SkillLocalData(tinyxml2::XMLElement *xmlElement)
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

SkillLocalData::~SkillLocalData()
{
    
}

int SkillLocalData::getId() const
{
    return _id;
}

const string& SkillLocalData::getName() const
{
    return _name;
}
