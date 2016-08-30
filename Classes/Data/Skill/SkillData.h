//
//  SkillData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SkillData_h
#define SkillData_h

#include "json/document.h"

class SkillData
{
public:
    SkillData(int level);
    virtual ~SkillData();

    int getLevel() const;
    
private:
    int _level;
};

#endif /* SkillData_h */
