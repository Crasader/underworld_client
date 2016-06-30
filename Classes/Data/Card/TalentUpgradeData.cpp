//
//  TalentUpgradeData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TalentUpgradeData.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

TalentUpgradeData::TalentUpgradeData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
,_gold(0)
,_book(0)
{
    if (xmlElement) {
        _id = atoi(xmlElement->Attribute("id"));
        _level = atoi(xmlElement->Attribute("level"));
        
        {
            const char *data = xmlElement->Attribute("gold");
            if (data) {
                _gold = atoi(data);
            }
        }
        
        {
            const char *data = xmlElement->Attribute("book");
            if (data) {
                _book = atoi(data);
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
    }
}

TalentUpgradeData::~TalentUpgradeData() {}

int TalentUpgradeData::getId() const
{
    return _id;
}

int TalentUpgradeData::getLevel() const
{
    return _level;
}

int TalentUpgradeData::getGold() const
{
    return _gold;
}

int TalentUpgradeData::getBook() const
{
    return _book;
}

const string& TalentUpgradeData::getName() const
{
    return _name;
}

const string& TalentUpgradeData::getDescription() const
{
    return _description;
}
