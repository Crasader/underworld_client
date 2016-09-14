//
//  CardProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardProperty_h
#define CardProperty_h

#include "AbstractProperty.h"
#include "CocosGlobal.h"
#include "ObjectUtils.h"
#include "GameModeHMM.h"
#include <vector>
#include <unordered_map>

namespace UnderWorld {
    namespace Core {
        class HMMCardType;
    }
}

class CardProperty : public AbstractProperty
{
public:
    explicit CardProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~CardProperty();
    
    UnderWorld::Core::HMMCardClass getCardClass() const;
    bool isHero() const;
    int getCost() const;
    int getRarity() const;
    int getBeUnlockedLevel() const;
    int getBeRequiredCount() const;
    int getMaxDonateCount() const;
    const std::pair<ResourceType, int>& getDonateReward() const;
    int getDonateExp() const;
    int getDonatePoint() const;
    const std::vector<int>& getSkills() const;
    const std::map<ObjectUtils::CardAttributeType, float>& getAttributes() const;
    float getAttribute(ObjectUtils::CardAttributeType type);
    ObjectUtils::AttackType getAttackType() const;
    ObjectUtils::ArmorType getArmorType() const;
    const std::vector<ObjectUtils::TargetType>& getTargetTypes() const;
    ObjectUtils::RuneType getRuneType(int idx) const;
    
private:
    const UnderWorld::Core::HMMCardType* _cardType;
    int _rarity;
    int _beUnlockedLevel;
    int _beRequiredCount;
    int _maxDonateCount;
    std::pair<ResourceType, int> _donateReward;
    int _donateExp;
    int _donatePoint;
    std::vector<int> _skills;
    std::map<ObjectUtils::CardAttributeType, float> _attributes;
    ObjectUtils::AttackType _attackType;
    ObjectUtils::ArmorType _armorType;
    std::vector<ObjectUtils::TargetType> _targetTypes;
    std::unordered_map<int, ObjectUtils::RuneType> _runeTypes;
};

#endif /* CardProperty_h */
