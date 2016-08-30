//
//  CardData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardData_h
#define CardData_h

#include "CardSimpleData.h"

class SkillData;
class RuneData;

class CardData : public CardSimpleData
{
public:
    CardData(const rapidjson::Value& jsonDict);
    virtual ~CardData();
    
    const std::vector<SkillData*>& getSkills() const;
    const std::vector<RuneData*>& getRunes() const;
    
protected:
    std::vector<SkillData*> _skills;
    std::vector<RuneData*> _runes;
};

#endif /* CardData_h */
