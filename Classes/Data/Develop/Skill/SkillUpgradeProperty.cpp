//
//  SkillUpgradeProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SkillUpgradeProperty.h"
#include "tinyxml2/tinyxml2.h"

SkillUpgradeProperty::SkillUpgradeProperty(tinyxml2::XMLElement *xmlElement)
:AbstractUpgradeProperty(xmlElement)
,_bookCost(0)
{
    attribute2Int(xmlElement, "book", _bookCost);
}

SkillUpgradeProperty::~SkillUpgradeProperty() {}

int SkillUpgradeProperty::getBookCost() const
{
    return _bookCost;
}
