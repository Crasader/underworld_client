//
//  LevelLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef LevelLocalData_h
#define LevelLocalData_h

#include "AbstractLocalData.h"
#include "CocosGlobal.h"
#include <vector>
#include <unordered_map>

class ConditionData;
class RewardData;

class LevelLocalData : public AbstractLocalData
{
public:
    LevelLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~LevelLocalData();
    
    const std::vector<ConditionData*>& getConditions() const;
    const RewardData* getReward(int type) const;
    
private:
    std::vector<ConditionData*> _conditions;
    std::unordered_map<int, RewardData*> _rewards;
};

#endif /* LevelLocalData_h */
