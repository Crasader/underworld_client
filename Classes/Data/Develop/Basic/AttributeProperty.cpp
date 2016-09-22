//
//  AttributeProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/22.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AttributeProperty.h"
#include "XMLUtils.h"

using namespace std;

AttributeProperty::AttributeProperty(tinyxml2::XMLElement *xmlElement)
:_id(0)
,_type(Type::ABSOLUTE)
,_priority(INT_MAX)
{
    if (xmlElement) {
        XMLUtils::parse(xmlElement, "id", _id);
        XMLUtils::parse(xmlElement, "type", _type);
        XMLUtils::parse(xmlElement, "enum_value", _enmuPrefix);
        XMLUtils::parse(xmlElement, "name", _name);
        XMLUtils::parse(xmlElement, "icon", _icon);
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

const string& AttributeProperty::getEnmuPrefix() const
{
    return _enmuPrefix;
}

const string& AttributeProperty::getName() const
{
    return _name;
}

const string& AttributeProperty::getIcon() const
{
    return _icon;
}
