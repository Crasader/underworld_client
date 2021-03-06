//
//  SkillProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/8/31.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SkillProperty_h
#define SkillProperty_h

#include "DevelopProperty.h"

class SkillProperty : public DevelopProperty
{
public:
    explicit SkillProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~SkillProperty();
    
    int getBeUnlockedLevel() const;
    
private:
    int _beUnlockedLevel;
};

#endif /* SkillProperty_h */
