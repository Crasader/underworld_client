//
//  BRConfigData.h
//  Underworld_Client
//
//  Created by Andy on 16/4/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BRConfigData_h
#define BRConfigData_h

#include <string>

#include "EffectData.h"

namespace tinyxml2 { class XMLElement; }

class BRConfigData
{
public:
    explicit BRConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~BRConfigData();
    
    const std::string& getName() const;
    const EffectData& getResource() const;
    const EffectData& getShadowResource() const;
    const EffectData& getExplodeResource() const;
    const std::string& getExplodeSound() const;
    
private:
    std::string _name;
    EffectData _resource;
    EffectData _shadowResource;
    EffectData _explodeResource;
    std::string _explodeSound;
};

#endif /* BRConfigData_h */
