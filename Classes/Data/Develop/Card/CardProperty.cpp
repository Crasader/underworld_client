//
//  CardProperty.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "CardProperty.h"
#include "Utils.h"
#include "XMLUtils.h"
#include "DataManager.h"
#include "GameConstants.h"

CardProperty::CardProperty(tinyxml2::XMLElement *xmlElement)
:AbstractProperty(xmlElement)
,_cardType(nullptr)
,_rarity(0)
,_beUnlockedLevel(0)
,_beRequiredCount(0)
,_maxDonateCount(0)
,_donateExp(0)
,_donatePoint(0)
{
    _cardType = DataManager::getInstance()->getGameModeHMM()->findCardTypeById(getId());
    if (_cardType) {
        _name = _cardType->getName();
    }
    
    XMLUtils::parse(xmlElement, "grade", _rarity);
    XMLUtils::parse(xmlElement, "unlock", _beUnlockedLevel);
    XMLUtils::parse(xmlElement, "require", _beRequiredCount);
    XMLUtils::parse(xmlElement, "donate", _maxDonateCount);
    XMLUtils::parseString(xmlElement, "reward", "_", [this](int idx, const std::string& split) {
        if (0 == idx) {
            _donateReward.first = static_cast<ResourceType>(atoi(split.c_str()));
        } else if (1 == idx) {
            _donateReward.second = atoi(split.c_str());
        }
    });
    XMLUtils::parse(xmlElement, "experience", _donateExp);
    XMLUtils::parse(xmlElement, "point", _donatePoint);
    XMLUtils::parseString(xmlElement, "skills", ";", [this](int idx, const std::string& split) {
        _skills.push_back(atoi(split.c_str()));
    });
    XMLUtils::parseString(xmlElement, "runes", ";", [this](int idx, const std::string& split) {
        auto type = static_cast<ObjectUtils::RuneType>(atoi(split.c_str()));
        if (ObjectUtils::RuneType::NONE != type) {
            _runeTypes.insert(std::make_pair(idx, type));
        }
    });
    
    {
        static const std::map<ObjectUtils::CardAttributeType, std::string> AttributeKeys {
            {ObjectUtils::CardAttributeType::HP, "hp"},
            {ObjectUtils::CardAttributeType::SPEED, "speed"},
            {ObjectUtils::CardAttributeType::ARMOR, "armor"},
            {ObjectUtils::CardAttributeType::ARMOR_TYPE, "armortype"},
            {ObjectUtils::CardAttributeType::GROUND_DAMAGE, "damage1"},
            {ObjectUtils::CardAttributeType::ATTACK_TYPE, "damagetype"},
            {ObjectUtils::CardAttributeType::HIT_SPEED, "hitspeed"},
            {ObjectUtils::CardAttributeType::RANGE, "range"},
            {ObjectUtils::CardAttributeType::TARGET_TYPE, "target"},
            {ObjectUtils::CardAttributeType::AIR_DAMAGE, "damage2"},
        };
        
        for (auto iter = begin(AttributeKeys); iter != end(AttributeKeys); ++iter) {
            auto key(iter->second.c_str());
            if (!xmlElement->Attribute(key)) { continue; }
            auto value = XMLUtils::parse<float>(xmlElement, key);
            auto type(iter->first);
            CC_BREAK_IF(ObjectUtils::CardAttributeType::HP == type && value <= 0);
            
            if (ObjectUtils::CardAttributeType::AIR_DAMAGE == type) {
                do {
                    auto targetTypeIter(_attributes.find(ObjectUtils::CardAttributeType::TARGET_TYPE));
                    CC_BREAK_IF(targetTypeIter == end(_attributes));
                    auto targetType(targetTypeIter->second);
                    CC_BREAK_IF(targetType == static_cast<float>(ObjectUtils::TargetType::GROUND));
                    const auto iter(_attributes.find(ObjectUtils::CardAttributeType::GROUND_DAMAGE));
                    if (value == 0 && iter != end(_attributes)) {
                        _attributes.insert(make_pair(type, iter->second));
                    } else {
                        _attributes.insert(make_pair(type, value));
                    }
                } while (false);
            } else if (ObjectUtils::CardAttributeType::ARMOR_TYPE == type ||
                       ObjectUtils::CardAttributeType::ATTACK_TYPE == type ||
                       ObjectUtils::CardAttributeType::TARGET_TYPE == type ||
                       value != 0) {
                _attributes.insert(make_pair(type, value));
            }
        }
    }
}

CardProperty::~CardProperty() {}

UnderWorld::Core::HMMCardClass CardProperty::getCardClass() const
{
    if (_cardType) {
        return _cardType->getCardClass();
    }
    
    return UnderWorld::Core::HMMCardClass::HHMCARD_CLASS_COUNT;
}

bool CardProperty::isHero() const
{
    return UnderWorld::Core::HMMCardClass::kHMMCardClass_Hero == getCardClass();
}

int CardProperty::getCost() const
{
    if (_cardType) {
        const auto& costs(_cardType->getCost());
        auto iter(costs.find(RES_NAME_WOOD));
        if (iter != end(costs)) {
            return UnderWorld::Core::GameConstants::microres2Res(iter->second);
        }
    }
    
    return 0;
}

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

const std::vector<int>& CardProperty::getSkills() const
{
    return _skills;
}

const std::map<ObjectUtils::CardAttributeType, float>& CardProperty::getAttributes() const
{
    return _attributes;
}

float CardProperty::getAttribute(ObjectUtils::CardAttributeType type)
{
    if (_attributes.find(type) != end(_attributes)) {
        return _attributes.at(type);
    }
    
    return 0.0f;
}

ObjectUtils::RuneType CardProperty::getRuneType(int idx) const
{
    if (_runeTypes.find(idx) != end(_runeTypes)) {
        return _runeTypes.at(idx);
    }
    
    return ObjectUtils::RuneType::NONE;
}
