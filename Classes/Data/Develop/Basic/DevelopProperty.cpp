//
//  DevelopProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/22.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "DevelopProperty.h"
#include "ObjectUtils.h"
#include "XMLUtils.h"
#include "Utils.h"
#include "DataManager.h"
#include "AttributeProperty.h"

DevelopProperty::DevelopProperty(tinyxml2::XMLElement *xmlElement)
:AbstractProperty(xmlElement)
{
    static const std::string PARAM_PREFIX("param_");
    for (int i = 0; ; ++i) {
        auto key = (PARAM_PREFIX + Utils::toString(i)).c_str();
        auto data(xmlElement->Attribute(key));
        CC_BREAK_IF(!data);
        const auto s = XMLUtils::parse<std::string>(xmlElement, key);
        std::vector<std::string> splits;
        Utils::split(splits, data, "_", "");
        if (2 == splits.size()) {
            auto attribute(atoi(splits.at(0).c_str()));
            _attributes.insert(std::make_pair(attribute, atof(splits.at(1).c_str())));
            _orderedAttributes.push_back(attribute);
        }
    }
    
    if (!_orderedAttributes.empty()) {
        std::sort(_orderedAttributes.begin(), _orderedAttributes.end(), [this](int c1, int c2) {
            auto priority1(getAttributePriority(c1));
            auto priority2(getAttributePriority(c2));
            if (priority1 == priority2) {
                return c1 < c2;
            }
            
            return priority1 < priority2;
        });
    }
}

DevelopProperty::~DevelopProperty() {}

const std::unordered_map<int, float>& DevelopProperty::getAttributes() const
{
    return _attributes;
}

float DevelopProperty::getAttribute(int type)
{
    if (_attributes.find(type) != end(_attributes)) {
        return _attributes.at(type);
    }
    
    return 0.0f;
}

const std::vector<int>& DevelopProperty::getOrderedAttributes() const
{
    return _orderedAttributes;
}

int DevelopProperty::getAttributePriority(int aid) const
{
    auto property(DataManager::getInstance()->getAttributeProperty(aid));
    return property ? property->getPriority() : INT_MAX;
}
