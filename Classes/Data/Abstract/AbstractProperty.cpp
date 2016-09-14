//
//  AbstractProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AbstractProperty.h"
#include "XMLUtils.h"

using namespace std;

AbstractProperty::AbstractProperty(tinyxml2::XMLElement *xmlElement)
:_id(0)
{
    if (xmlElement) {
        XMLUtils::parse(xmlElement, "id", _id);
        XMLUtils::parse(xmlElement, "name", _name);
        XMLUtils::parse(xmlElement, "desc", _description);
    }
}

AbstractProperty::~AbstractProperty() {}

int AbstractProperty::getId() const
{
    return _id;
}

const string& AbstractProperty::getName() const
{
    return _name;
}

const string& AbstractProperty::getDescription() const
{
    return _description;
}
