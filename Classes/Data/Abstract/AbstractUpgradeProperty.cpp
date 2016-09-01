//
//  AbstractUpgradeProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AbstractUpgradeProperty.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ObjectBriefData.h"

using namespace std;

AbstractUpgradeProperty::AbstractUpgradeProperty(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_level(0)
{
    if (xmlElement) {
        attribute2Int(xmlElement, "id", _id);
        attribute2Int(xmlElement, "level", _level);
        
        int gold(0);
        attribute2Int(xmlElement, "gold", gold);
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

int AbstractUpgradeProperty::getResourceCost(ResourceType type) const
{
    if (_cost.find(type) != end(_cost)) {
        return _cost.at(type);
    }
    
    return 99999;
}

void AbstractUpgradeProperty::attribute2Int(tinyxml2::XMLElement* xmlElement, const char* key, int& output) const
{
    if (xmlElement && key && strlen(key) > 0) {
        const char* data(xmlElement->Attribute(key));
        if (data) {
            output = atoi(data);
        }
    }
}
