//
//  AchievementManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "AchievementManager.h"
#include "JSonUtils.h"
#include "Utils.h"
#include "DataManager.h"
#include "AchievementData.h"

using namespace std;

AchievementManager::AchievementManager()
{
    
}

AchievementManager::~AchievementManager()
{
    Utils::clearVector(_achievementData);
}

void AchievementManager::init(const rapidjson::Value& jsonDict)
{
    if (JSonUtils::isExist(jsonDict, "data"))
    {
        const rapidjson::Value& quests = DICTOOL->getSubDictionary_json(jsonDict, "data");
        const int cnt = DICTOOL->getArrayCount_json(jsonDict, "data");
        for (int i = 0; i < cnt; ++i)
        {
            const rapidjson::Value& info = DICTOOL->getSubDictionary_json(quests, i);
            AchievementData* data = new (nothrow) AchievementData(info);
            _achievementData.push_back(data);
        }
    }
}

void AchievementManager::updateProgress(int achievementId, int progress)
{
    
}

void AchievementManager::getReward(int achievementId)
{
    
}

const vector<AchievementData*>& AchievementManager::getData() const
{
    return _achievementData;
}

int AchievementManager::getProgress(int achievementId) const
{
    for (int i = 0; i < _achievementData.size(); ++i) {
        AchievementData* data = _achievementData.at(i);
        if (achievementId == data->getId()) {
            return data->getProgress();
        }
    }
    
    return 0;
}

void AchievementManager::finishAchievement(int achievementId)
{
    
}
