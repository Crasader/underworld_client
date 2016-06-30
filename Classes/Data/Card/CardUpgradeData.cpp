//
//  CardUpgradeData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardUpgradeData.h"
#include "tinyxml2/tinyxml2.h"

CardUpgradeData::CardUpgradeData(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
,_gold(0)
,_card(0)
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
            const char *data = xmlElement->Attribute("card");
            if (data) {
                _card = atoi(data);
            }
        }
    }
}

CardUpgradeData::~CardUpgradeData() {}

int CardUpgradeData::getId() const
{
    return _id;
}

int CardUpgradeData::getLevel() const
{
    return _level;
}

int CardUpgradeData::getGold() const
{
    return _gold;
}

int CardUpgradeData::getCard() const
{
    return _card;
}
