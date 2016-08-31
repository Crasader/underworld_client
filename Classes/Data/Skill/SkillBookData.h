//
//  SkillBookData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SkillBookData_h
#define SkillBookData_h

#include "json/document.h"

class SkillBookData
{
public:
    SkillBookData(const rapidjson::Value& jsonDict);
    virtual ~SkillBookData();
    
    void update(const rapidjson::Value& jsonDict);
    int getId() const;
    int getAmount() const;
    
private:
    int _id;
    int _amount;
};

#endif /* SkillBookData_h */
