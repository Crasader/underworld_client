//
//  CardData.h
//  Underworld_Client
//
//  Created by Andy on 16/8/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardData_h
#define CardData_h

#include "AbstractData.h"
#include <vector>
#include <unordered_map>

class SkillData;
class RuneData;

class CardData : public AbstractData
{
public:
    CardData(const rapidjson::Value& jsonDict);
    virtual ~CardData();
    
    void update(const rapidjson::Value& jsonDict);
    void removeRune(int idx);
    void updateRune(int idx, const rapidjson::Value& jsonDict);
    const std::vector<SkillData*>& getSkills() const;
    const RuneData* getRune(int idx) const;
    
protected:
    std::vector<SkillData*> _skills;
    std::unordered_map<int, RuneData*> _runes;
};

#endif /* CardData_h */
