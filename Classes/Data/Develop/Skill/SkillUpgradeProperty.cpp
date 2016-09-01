//
//  SkillUpgradeProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SkillUpgradeProperty.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

SkillUpgradeProperty::SkillUpgradeProperty(tinyxml2::XMLElement *xmlElement)
:AbstractUpgradeProperty(xmlElement)
{
    auto data = xmlElement->Attribute("book");
    if (data) {
        std::vector<std::string> splits;
        Utils::split(splits, data, "_");
        if (splits.size() > 1) {
            _bookCost.first = atoi(splits.at(0).c_str());
            _bookCost.second = atoi(splits.at(1).c_str());
        }
    }
}

SkillUpgradeProperty::~SkillUpgradeProperty() {}

const std::pair<int, int>& SkillUpgradeProperty::getBookCost() const
{
    return _bookCost;
}
