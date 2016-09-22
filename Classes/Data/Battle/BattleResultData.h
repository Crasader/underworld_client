//
//  BattleResultData.h
//  Underworld_Client
//
//  Created by Andy on 16/9/21.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BattleResultData_h
#define BattleResultData_h

#include <vector>
#include "json/document.h"
#include "CocosGlobal.h"

class ObjectBriefData;

class BattleResultData
{
public:
    explicit BattleResultData(const rapidjson::Value& jsonDict);
    virtual ~BattleResultData();
    
    enum class Result { WIN, DRAW, LOSE };
    
    Result getResult() const;
    int getTrophy() const;
    const std::vector<ObjectBriefData*>& getRewards() const;
    
private:
    Result _result;
    int _trophy;
    std::vector<ObjectBriefData*> _rewards;
};
#endif /* BattleResultData_h */
