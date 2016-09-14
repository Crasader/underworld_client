//
//  CardUpgradeProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardUpgradeProperty.h"
#include "XMLUtils.h"

CardUpgradeProperty::CardUpgradeProperty(tinyxml2::XMLElement *xmlElement)
:AbstractUpgradeProperty(xmlElement)
,_cardCost(0)
{
    XMLUtils::parse(xmlElement, "card", _cardCost);
}

CardUpgradeProperty::~CardUpgradeProperty() {}

int CardUpgradeProperty::getCardCost() const
{
    return _cardCost;
}
