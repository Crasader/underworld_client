//
//  LevelLocalData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/12.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef LevelLocalData_h
#define LevelLocalData_h

#include <map>
#include "CocosGlobal.h"

namespace tinyxml2 { class XMLElement; }

class ConditionData;
class RewardData;

class LevelLocalData
{
public:
    LevelLocalData(tinyxml2::XMLElement *xmlElement);
    virtual ~LevelLocalData();
    
    int getLevelId() const;
    const std::vector<ConditionData*>& getConditions() const;
    const std::vector<RewardData*>& getRewards() const;
    
private:
    int _levelId;
    std::vector<ConditionData*> _conditions;
    std::vector<RewardData*> _rewards;
};

#endif /* LevelLocalData_h */
