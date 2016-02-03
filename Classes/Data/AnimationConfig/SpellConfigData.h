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
#include <map>
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
    }SpellDirection;
    
    typedef enum
    {
        kFoot = 0,
        kBody = 1,
        kHead = 2,
    }SpellPosition;
    
public:
    SpellConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~SpellConfigData();
    
    // key
    const std::string& getSpellName() const;
    
    // caster params
    const std::vector<std::string>& getCasterResourceNames() const;
    SpellPosition getCasterSpellPosition() const;
    bool isCasterShakeScreen() const;
    
    // receiver params
    const std::vector<std::string>& getReceiverResourceNames() const;
    SpellDirection getReceiverSpellDirection() const;
    SpellPosition getReceiverSpellPosition() const;
    const std::map<UnderWorld::Core::SkillClass, float>& getReceiverSpeedRates() const;
    float getReceiverSpeedRate(UnderWorld::Core::SkillClass sc) const;
    const std::map<UnderWorld::Core::SkillClass, float>& getReceiverVolumeRates() const;
    float getReceiverVolumeRate(UnderWorld::Core::SkillClass sc) const;
    
private:
    std::string _spellName;
    
    std::vector<std::string> _casterResourceNames;
    SpellPosition _casterSpellPosition;
    bool _isCasterShakeScreen;
    
    std::vector<std::string> _receiverResourceNames;
    SpellDirection _receiverSpellDirection;
    SpellPosition _receiverSpellPosition;
    std::map<UnderWorld::Core::SkillClass, float> _receiverSpeedRates;
    std::map<UnderWorld::Core::SkillClass, float> _receiverVolumeRates;
};

#endif /* SpellConfigData_h */
