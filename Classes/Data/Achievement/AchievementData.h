//
//  AchievementData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AchievementData_h
#define AchievementData_h

#include "QuestData.h"

class AchievementLocalData;

class AchievementData
{
public:
    AchievementData(const rapidjson::Value& jsonDict);
    virtual ~AchievementData();
    
    int getId() const;
    const AchievementLocalData* getLocalData() const;
    int getProgress() const;
    int getStatus() const;
    
private:
    int _id;
    int _progress;
    int _status;
};

#endif /* AchievementData_h */
