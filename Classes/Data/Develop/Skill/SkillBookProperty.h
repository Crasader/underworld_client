//
//  SkillBookProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/9/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SkillBookProperty_h
#define SkillBookProperty_h

#include "AbstractProperty.h"

class SkillBookProperty : public AbstractProperty
{
public:
    explicit SkillBookProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~SkillBookProperty();
    
    int getBeUnlockedLevel() const;
    
private:
    int _beUnlockedLevel;
};

#endif /* SkillBookProperty_h */
