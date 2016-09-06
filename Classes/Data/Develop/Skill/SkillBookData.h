//
//  SkillBookData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SkillBookData_h
#define SkillBookData_h

#include "AbstractData.h"

class SkillBookData : public AbstractData
{
public:
    SkillBookData(const rapidjson::Value& jsonDict);
    virtual ~SkillBookData();
};

#endif /* SkillBookData_h */
