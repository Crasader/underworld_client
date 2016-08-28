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

namespace tinyxml2 { class XMLElement; }

class BRConfigData
{
public:
    BRConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~BRConfigData();
    
    const std::string& getName() const;
    const std::string& getResource() const;
    const std::string& getShadowResource() const;
    const std::string& getExplodeResource() const;
    const std::string& getExplodeSound() const;
    float getScaleX() const;
    float getScaleY() const;
    
private:
    std::string _name;
    std::string _resource;
    std::string _shadowResource;
    std::string _explodeResource;
    std::string _explodeSound;
    float _scaleX;
    float _scaleY;
};

#endif /* BRConfigData_h */
