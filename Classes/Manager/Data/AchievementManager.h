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
#include <vector>

class AchievementData;

class AchievementManager
{
public:
    static AchievementManager* getInstance();
    static void purge();
    
    void init(const rapidjson::Value& jsonDict);
    void updateProgress(int achievementId, int progress);
    void getReward(int achievementId);
    
    // getters
    const std::vector<AchievementData*>& getData() const;
    int getProgress(int achievementId) const;
    
protected:
    AchievementManager();
    virtual ~AchievementManager();
    AchievementManager(const AchievementManager &) = delete;
    AchievementManager &operator =(const AchievementManager &) = delete;
    void finish(int achievementId);
    
private:
    std::vector<AchievementData*> _achievements;
};

#endif /* AchievementManager_h */
