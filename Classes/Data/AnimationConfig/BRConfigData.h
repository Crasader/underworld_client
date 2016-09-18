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
#include <vector>

#include "EffectData.h"

namespace tinyxml2 { class XMLElement; }

enum class BulletMaterial {
    Arrow,
    Cannon
};

class BRConfigData
{
public:
    static const std::string TAIL_GAS_SPLITOR;
public:
    explicit BRConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~BRConfigData();
    
    const std::string& getName() const;
    const EffectData& getResource() const;
    const EffectData& getShadowResource() const;
    const EffectData& getExplodeResource() const;
    const std::string& getExplodeSound() const;
    BulletMaterial getBulletMeterial() const;
    const std::vector<EffectData>& getTailGasResource() const;
    int getTailGasInterval() const;
    
private:
    std::string _name;
    EffectData _resource;
    EffectData _shadowResource;
    EffectData _explodeResource;
    std::string _explodeSound;
    BulletMaterial _material;
    std::vector<EffectData> _tailGas;
    int _tailGasInterval;
};

#endif /* BRConfigData_h */
