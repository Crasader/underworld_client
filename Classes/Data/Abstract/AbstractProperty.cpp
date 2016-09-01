//
//  AbstractProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/1/7.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AbstractProperty.h"
#include "tinyxml2/tinyxml2.h"

using namespace std;

AbstractProperty::AbstractProperty(tinyxml2::XMLElement *xmlElement)
:_id(0)
{
    if (xmlElement) {
        attribute2Int(xmlElement, "id", _id);
        attribute2String(xmlElement, "name", _name);
        attribute2String(xmlElement, "desc", _description);
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

void AbstractProperty::attribute2Int(tinyxml2::XMLElement* xmlElement, const char* key, int& output) const
{
    if (xmlElement && key && strlen(key) > 0) {
        const char* data(xmlElement->Attribute(key));
        if (data) {
            output = atoi(data);
        }
    }
}

void AbstractProperty::attribute2String(tinyxml2::XMLElement* xmlElement, const char* key, string& output) const
{
    if (xmlElement && key && strlen(key) > 0) {
        const char* data(xmlElement->Attribute(key));
        if (data) {
            output.assign(data);
        }
    }
}
