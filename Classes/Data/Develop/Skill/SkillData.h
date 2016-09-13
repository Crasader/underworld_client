//
//  SkillData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SkillData_h
#define SkillData_h

#include "AbstractData.h"

class SkillData : public AbstractData
{
public:
    explicit SkillData(int skillId, int level);
    virtual ~SkillData();
    
    void update(int level);
};

#endif /* SkillData_h */
