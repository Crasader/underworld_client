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
#include <unordered_map>

class CardProperty : public AbstractProperty
{
public:
    CardProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~CardProperty();
    
    int getRarity() const;
    int getBeUnlockedLevel() const;
    int getBeRequiredCount() const;
    int getMaxDonateCount() const;
    const std::pair<ResourceType, int>& getDonateReward() const;
    int getDonateExp() const;
    int getDonatePoint() const;
    RuneType getRuneType(int idx) const;
    
private:
    int _rarity;
    int _beUnlockedLevel;
    int _beRequiredCount;
    int _maxDonateCount;
    std::pair<ResourceType, int> _donateReward;
    int _donateExp;
    int _donatePoint;
    std::unordered_map<int, RuneType> _runeTypes;
};

#endif /* CardProperty_h */
