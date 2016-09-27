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
#include "AchievementData.h"

using namespace std;

static AchievementManager* s_pInstance(nullptr);
AchievementManager* AchievementManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) AchievementManager();
    }
    
    return s_pInstance;
}

void AchievementManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

AchievementManager::AchievementManager() {}

AchievementManager::~AchievementManager()
{
    Utils::clearVector(_achievements);
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
            _achievements.push_back(data);
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
    return _achievements;
}

int AchievementManager::getProgress(int achievementId) const
{
    for (int i = 0; i < _achievements.size(); ++i) {
        AchievementData* data = _achievements.at(i);
        if (achievementId == data->getId()) {
            return data->getProgress();
        }
    }
    
    return 0;
}

void AchievementManager::finish(int achievementId)
{
    
}
