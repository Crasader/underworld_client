//
//  SkillData.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "SkillData.h"

SkillData::SkillData(int level)
:_level(level) {}

SkillData::~SkillData() {}

int SkillData::getLevel() const
{
    return _level;
}
