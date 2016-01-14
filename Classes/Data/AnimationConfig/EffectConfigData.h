//
//  EffectConfigData.h
//  Underworld_Client
//
//  Created by Andy on 16/1/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef EffectConfigData_h
#define EffectConfigData_h

#include <iostream>

namespace tinyxml2 { class XMLElement; }

class EffectConfigData
{
public:
    EffectConfigData(tinyxml2::XMLElement *xmlElement);
    virtual ~EffectConfigData();
    
    const std::string& getName() const;
    
private:
    std::string _name;
};

#endif /* EffectConfigData_h */
