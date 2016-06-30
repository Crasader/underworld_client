//
//  CardLocalData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardLocalData.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

CardLocalData::CardLocalData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_type(0)
,_status(0)
,_food(0)
,_quality(0)
{
    if (xmlElement) {
        _id = atoi(xmlElement->Attribute("id"));
        _type = atoi(xmlElement->Attribute("type"));
        
        {
            const char *data = xmlElement->Attribute("status");
            if (data) {
                _status = atoi(data);
            }
        }
        
        {
            const char *data = xmlElement->Attribute("food");
            if (data) {
                _food = atoi(data);
            }
        }
        
        {
            const char *data = xmlElement->Attribute("quality");
            if (data) {
                _quality = atoi(data);
            }
        }
        
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
        
        {
            const char *data = xmlElement->Attribute("unlock");
            if (data) {
                _unlockInfo.assign(data);
            }
        }
    }
}

CardLocalData::~CardLocalData() {}

int CardLocalData::getId() const
{
    return _id;
}

int CardLocalData::getType() const
{
    return _type;
}

int CardLocalData::getStatus() const
{
    return _status;
}

int CardLocalData::getFood() const
{
    return _food;
}

int CardLocalData::getQuality() const
{
    return _quality;
}

const string& CardLocalData::getName() const
{
    return _name;
}

const string& CardLocalData::getDescription() const
{
    return _description;
}

const string& CardLocalData::getUnlockInfo() const
{
    return _unlockInfo;
}
