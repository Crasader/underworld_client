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
    SkillData(int skillId, int level);
    virtual ~SkillData();
    
    virtual const AbstractProperty* getProperty() const override;
    
    // TODO: remove test code
    virtual const std::string& getName() const override;
    virtual const std::string& getDescription() const override;
};

#endif /* SkillData_h */
