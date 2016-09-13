//
//  MapParticleConfigData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/18.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef MapParticleConfigData_h
#define MapParticleConfigData_h

#include <iostream>

namespace tinyxml2 { class XMLElement; }

class MapParticleConfigData
{
public:
    explicit MapParticleConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~MapParticleConfigData();
    
    const std::string& getName() const;
    float getPosX() const;
    float getPosY() const;
    float getScaleX() const;
    float getScaleY() const;
    
private:
    std::string _name;
    float _posX;
    float _posY;
    float _scaleX;
    float _scaleY;
};

#endif /* MapParticleConfigData_h */
