//
//  QuestManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestManager.h"
#include "DataManager.h"
#include "QuestLocalData.h"
#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocostudio;

QuestManager::QuestManager()
{
    
}

QuestManager::~QuestManager()
{
    
}

void QuestManager::initQuest(QuestType type, const rapidjson::Value& jsonDict)
{
    if (_questData.find(type) != _questData.end()) {
        _questData.at(type).clear();
    } else {
        _questData.insert(make_pair(type, vector<const QuestLocalData*>()));
    }
    
    if (_questProgress.find(type) != _questProgress.end()) {
        _questProgress.at(type).clear();
    } else {
        _questProgress.insert(make_pair(type, map<int, QuestProgress>()));
    }
    
    vector<const QuestLocalData*>& questDataVector = _questData.at(type);
    map<int, QuestProgress>& progressMap = _questProgress.at(type);
    if (DICTOOL->checkObjectExist_json(jsonDict, "data"))
    {
        const rapidjson::Value& quests = DICTOOL->getSubDictionary_json(jsonDict, "data");
        const int cnt = DICTOOL->getArrayCount_json(jsonDict, "data");
        for (int i = 0; i < cnt; ++i)
        {
            const rapidjson::Value& info = DICTOOL->getSubDictionary_json(quests, i);
            int questId = DICTOOL->getIntValue_json(info, "qid");
            int progress = DICTOOL->getIntValue_json(info, "progress");
            const QuestLocalData* data = DataManager::getInstance()->getQuestData(type, questId);
            if (data) {
                questDataVector.push_back(data);
                progressMap.insert(make_pair(questId, make_pair(data, progress)));
            } else {
                assert(false);
            }
        }
    }
}

void QuestManager::updateQuestProgress(QuestType type, int questId, int progress)
{
    if (_questProgress.find(type) != _questProgress.end()) {
        map<int, QuestProgress>& map = _questProgress.at(type);
        const QuestLocalData* data = DataManager::getInstance()->getQuestData(type, questId);
        if (data) {
            // set progress
            if (map.find(questId) != map.end()) {
                assert(data == map.at(questId).first);
                map.at(questId).second = progress;
            } else {
                map.insert(make_pair(questId, make_pair(data, progress)));
            }
            
            // check if the quest if finished
            
        } else {
            assert(false);
        }
    }
}

void QuestManager::getReward(QuestType type, int questId)
{
    
}

const vector<const QuestLocalData*>& QuestManager::getQuestData(QuestType type)
{
    if (_questData.find(type) == _questData.end()) {
        _questData.insert(make_pair(type, vector<const QuestLocalData*>()));
    }
    
    return _questData.at(type);
}

int QuestManager::getQuestProgress(QuestType type, int questId) const
{
    if (_questProgress.find(type) != _questProgress.end()) {
        const map<int, QuestProgress>& map = _questProgress.at(type);
        if (map.find(questId) != map.end()) {
            return map.at(questId).second;
        }
    }
    
    return 0;
}

void QuestManager::finishQuest(QuestType type, int questId)
{
    
}