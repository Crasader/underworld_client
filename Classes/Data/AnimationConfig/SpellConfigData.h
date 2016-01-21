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

namespace tinyxml2 { class XMLElement; }

class SpellConfigData
{
public:
    SpellConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~SpellConfigData();
    
    const std::string& getSpellName() const;
    const std::vector<std::string>& getCasterResourceNames() const;
    bool isCasterEffectOnBody() const;
    float getCasterVolumeRate() const;
    const std::vector<std::string>& getReceiverResourceNames() const;
    bool isReceiverEffectOnBody() const;
    float getReceiverVolumeRate() const;
    
private:
    std::string _spellName;
    std::vector<std::string> _casterResourceNames;
    bool _isCasterEffectOnBody;
    float _casterVolumeRate;
    std::vector<std::string> _resourceNames;
    bool _isReceiverEffectOnBody;
    float _receiverVolumeRate;
};

#endif /* SpellConfigData_h */
