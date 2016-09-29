//
//  AttributeProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/22.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AttributeProperty.h"
#include "Utils.h"
#include "XMLUtils.h"

AttributeProperty::AttributeProperty(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_type(Type::ABSOLUTE)
,_priority(INT_MAX)
,_relative(0, 0)
{
    if (xmlElement) {
        XMLUtils::parse(xmlElement, "id", _id);
        XMLUtils::parse(xmlElement, "type", _type);
        XMLUtils::parse(xmlElement, "enum_value", _enmuPrefix);
        XMLUtils::parse(xmlElement, "name", _name);
        XMLUtils::parse(xmlElement, "icon", _icon);
        XMLUtils::parse(xmlElement, "suffix", _suffix);
        
        XMLUtils::parseString(xmlElement, "relative", ":", [this](int idx, const std::string& split) {
            if (0 == idx) {
                _relative.first = Utils::stoi(split);
            } else if (1 == idx) {
                _relative.second = Utils::stoi(split);
            }
        });
    }
}

AttributeProperty::~AttributeProperty() {}

int AttributeProperty::getId() const
{
    return _id;
}

AttributeProperty::Type AttributeProperty::getType() const
{
    return _type;
}

int AttributeProperty::getPriority() const
{
    return _priority;
}

const std::pair<int, int>& AttributeProperty::getRelative() const
{
    return _relative;
}

const std::string& AttributeProperty::getEnmuPrefix() const
{
    return _enmuPrefix;
}

const std::string& AttributeProperty::getName() const
{
    return _name;
}

const std::string& AttributeProperty::getIcon() const
{
    return _icon;
}

const std::string& AttributeProperty::getSuffix() const
{
    return _suffix;
}
