//
//  SkillLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/28.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SkillLocalData_h
#define SkillLocalData_h

#include "AbstractLocalData.h"

class SkillLocalData : public AbstractLocalData
{
public:
    SkillLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~SkillLocalData();
};

#endif /* SkillLocalData_h */
