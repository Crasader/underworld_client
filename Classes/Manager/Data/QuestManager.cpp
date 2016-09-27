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
#include "QuestData.h"

using namespace std;

static QuestManager* s_pInstance(nullptr);
QuestManager* QuestManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) QuestManager();
    }
    
    return s_pInstance;
}

void QuestManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

QuestManager::QuestManager() {}

QuestManager::~QuestManager()
{
    for (auto iter = _quests.begin(); iter != _quests.end(); ++iter) {
        Utils::clearVector(iter->second);
    }
}

void QuestManager::init(QuestType type, const rapidjson::Value& jsonDict)
{
    auto iter(_quests.find(type));
    if (iter != _quests.end()) {
        Utils::clearVector(iter->second);
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

void QuestManager::updateProgress(QuestType type, int questId, int progress)
{
    
}

void QuestManager::getReward(QuestType type, int questId)
{
    
}

const vector<QuestData*>& QuestManager::getData(QuestType type)
{
    if (0 == _quests.count(type)) {
        _quests.insert(make_pair(type, vector<QuestData*>()));
    }
    
    return _quests.at(type);
}

int QuestManager::getProgress(QuestType type, int questId) const
{
    auto iter(_quests.find(type));
    if (iter != _quests.end()) {
        const auto& v(iter->second);
        for (int i = 0; i < v.size(); ++i) {
            QuestData* data = v.at(i);
            if (data->getType() == type && data->getId() == questId) {
                return data->getProgress();
            }
        }
    }
    
    return 0;
}

void QuestManager::finish(QuestType type, int questId)
{
    
}