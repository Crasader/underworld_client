//
//  QuestManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestManager.h"
#include "JSonUtils.h"
#include "Utils.h"
#include "DataManager.h"
#include "QuestData.h"

using namespace std;

QuestManager::QuestManager()
{
    
}

QuestManager::~QuestManager()
{
    for (auto iter = _quests.begin(); iter != _quests.end(); ++iter) {
        Utils::clearVector(iter->second);
    }
}

void QuestManager::initQuest(QuestType type, const rapidjson::Value& jsonDict)
{
    if (_quests.find(type) != _quests.end()) {
        Utils::clearVector(_quests.at(type));
    } else {
        _quests.insert(make_pair(type, vector<QuestData*>()));
    }
    
    vector<QuestData*>& questDataVector = _quests.at(type);
    if (JSonUtils::isExist(jsonDict, "data"))
    {
        const rapidjson::Value& quests = DICTOOL->getSubDictionary_json(jsonDict, "data");
        const int cnt = DICTOOL->getArrayCount_json(jsonDict, "data");
        for (int i = 0; i < cnt; ++i)
        {
            const rapidjson::Value& info = DICTOOL->getSubDictionary_json(quests, i);
            QuestData* data = new (nothrow) QuestData(info);
            questDataVector.push_back(data);
        }
    }
}

void QuestManager::updateQuestProgress(QuestType type, int questId, int progress)
{
    
}

void QuestManager::getReward(QuestType type, int questId)
{
    
}

const vector<QuestData*>& QuestManager::getQuestData(QuestType type)
{
    if (_quests.find(type) == _quests.end()) {
        _quests.insert(make_pair(type, vector<QuestData*>()));
    }
    
    return _quests.at(type);
}

int QuestManager::getQuestProgress(QuestType type, int questId) const
{
    if (_quests.find(type) != _quests.end()) {
        const vector<QuestData*>& v = _quests.at(type);
        for (int i = 0; i < v.size(); ++i) {
            QuestData* data = v.at(i);
            if (data->getType() == type && data->getId() == questId) {
                return data->getProgress();
            }
        }
    }
    
    return 0;
}

void QuestManager::finishQuest(QuestType type, int questId)
{
    
}