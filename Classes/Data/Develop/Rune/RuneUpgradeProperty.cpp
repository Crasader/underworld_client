//
//  RuneUpgradeProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "RuneUpgradeProperty.h"
#include "XMLUtils.h"

RuneUpgradeProperty::RuneUpgradeProperty(tinyxml2::XMLElement *xmlElement)
:AbstractUpgradeProperty(xmlElement)
,_runeCost(0)
{
    XMLUtils::parse(xmlElement, "rune", _runeCost);
}

RuneUpgradeProperty::~RuneUpgradeProperty() {}

int RuneUpgradeProperty::getRuneCost() const
{
    return _runeCost;
}
