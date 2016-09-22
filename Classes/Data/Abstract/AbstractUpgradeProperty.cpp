//
//  AbstractUpgradeProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AbstractUpgradeProperty.h"
#include "Utils.h"
#include "XMLUtils.h"

using namespace std;

AbstractUpgradeProperty::AbstractUpgradeProperty(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
{
    if (xmlElement) {
        XMLUtils::parse(xmlElement, "id", _id);
        XMLUtils::parse(xmlElement, "level", _level);
        
        auto gold = XMLUtils::parse<int>(xmlElement, "gold");
        if (gold > 0) {
            _cost.insert(make_pair(ResourceType::Gold, gold));
        }
    }
}

AbstractUpgradeProperty::~AbstractUpgradeProperty() {}

int AbstractUpgradeProperty::getId() const
{
    return _id;
}

int AbstractUpgradeProperty::getLevel() const
{
    return _level;
}

pair<ResourceType, int> AbstractUpgradeProperty::getResourceCost() const
{
    if (_cost.size() > 0) {
        return *(_cost.begin());
    }
    
    return make_pair(ResourceType::MAX, 0);
}
