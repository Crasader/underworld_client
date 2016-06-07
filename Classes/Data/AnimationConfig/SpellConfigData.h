//
//  SpellConfigData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SpellConfigData_h
#define SpellConfigData_h

#include <iostream>
#include <vector>
#include <unordered_map>
#include "SkllType.h"

namespace tinyxml2 { class XMLElement; }

class SpellConfigData
{
public:
    typedef enum
    {
        kNone = 0,
        kLeft = 1,
        kRight = 2,
    }Direction;
    
    typedef enum
    {
        kFoot = 0,
        kBody = 1,
        kHead = 2,
    }Position;
    
public:
    SpellConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~SpellConfigData();
    
    // key
    const std::string& getName() const;
    
    // caster params
    const std::vector<std::string>& getCasterResourceNames() const;
    Position getCasterSpellPosition() const;
    bool isCasterShakeScreen() const;
    
    // receiver params
    const std::vector<std::string>& getReceiverResourceNames() const;
    Direction getReceiverSpellDirection() const;
    Position getReceiverSpellPosition() const;
    const std::map<UnderWorld::Core::SkillClass, float>& getReceiverSpeedRates() const;
    float getReceiverSpeedRate(UnderWorld::Core::SkillClass sc) const;
    const std::map<UnderWorld::Core::SkillClass, float>& getReceiverVolumeRates() const;
    float getReceiverVolumeRate(UnderWorld::Core::SkillClass sc) const;
    
private:
    std::string _spellName;
    
    std::vector<std::string> _casterResourceNames;
    Position _casterSpellPosition;
    bool _isCasterShakeScreen;
    
    std::vector<std::string> _receiverResourceNames;
    Direction _receiverSpellDirection;
    Position _receiverSpellPosition;
    std::map<UnderWorld::Core::SkillClass, float> _receiverSpeedRates;
    std::map<UnderWorld::Core::SkillClass, float> _receiverVolumeRates;
};

#endif /* SpellConfigData_h */
