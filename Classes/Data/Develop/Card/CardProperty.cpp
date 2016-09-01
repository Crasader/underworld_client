//
//  CardProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardProperty.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"

CardProperty::CardProperty(tinyxml2::XMLElement *xmlElement)
:AbstractProperty(xmlElement)
,_rarity(0)
,_beUnlockedLevel(0)
,_beRequiredCount(0)
,_maxDonateCount(0)
,_donateExp(0)
,_donatePoint(0)
{
    attribute2Int(xmlElement, "grade", _rarity);
    attribute2Int(xmlElement, "unlock", _beUnlockedLevel);
    attribute2Int(xmlElement, "require", _beRequiredCount);
    attribute2Int(xmlElement, "donate", _maxDonateCount);
    
    {
        auto data = xmlElement->Attribute("reward");
        if (data) {
            std::vector<std::string> splits;
            Utils::split(splits, data, "_");
            if (splits.size() > 1) {
                _donateReward.first = static_cast<ResourceType>(atoi(splits.at(0).c_str()));
                _donateReward.second = atoi(splits.at(1).c_str());
            }
        }
    }
    
    attribute2Int(xmlElement, "experience", _donateExp);
    attribute2Int(xmlElement, "point", _donatePoint);
    
    {
        auto data = xmlElement->Attribute("runes");
        if (data) {
            std::vector<std::string> splits;
            Utils::split(splits, data, ";");
            for (int i = 0; i < splits.size(); ++i) {
                auto type = static_cast<RuneType>(atoi(splits.at(i).c_str()));
                if (RuneType::None != type) {
                    _runeTypes.insert(std::make_pair(i, type));
                }
            }
        }
    }
}

CardProperty::~CardProperty() {}

int CardProperty::getRarity() const
{
    return _rarity;
}

int CardProperty::getBeUnlockedLevel() const
{
    return _beUnlockedLevel;
}

int CardProperty::getBeRequiredCount() const
{
    return _beRequiredCount;
}

int CardProperty::getMaxDonateCount() const
{
    return _maxDonateCount;
}

const std::pair<ResourceType, int>& CardProperty::getDonateReward() const
{
    return _donateReward;
}

int CardProperty::getDonateExp() const
{
    return _donateExp;
}

int CardProperty::getDonatePoint() const
{
    return _donatePoint;
}

RuneType CardProperty::getRuneType(int idx) const
{
    if (_runeTypes.find(idx) != end(_runeTypes)) {
        return _runeTypes.at(idx);
    }
    
    return RuneType::None;
}
