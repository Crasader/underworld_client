//
//  AchievementManager.h
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef AchievementManager_h
#define AchievementManager_h

#include "json/document.h"
#include "CocosGlobal.h"
#include <vector>

class AchievementData;

class AchievementManager
{
public:
    AchievementManager();
    virtual ~AchievementManager();
    
    void init(const rapidjson::Value& jsonDict);
    void updateProgress(int achievementId, int progress);
    void getReward(int achievementId);
    
    // getters
    const std::vector<AchievementData*>& getData() const;
    int getProgress(int achievementId) const;
    
protected:
    M_DISALLOW_COPY_AND_ASSIGN(AchievementManager);
    void finishAchievement(int achievementId);
    
private:
    std::vector<AchievementData*> _achievementData;
};

#endif /* AchievementManager_h */
