//
//  QuestManager.h
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestManager_h
#define QuestManager_h

#include "json/document.h"
#include <vector>
#include <map>
#include "CocosGlobal.h"

class QuestData;

class QuestManager
{
public:
    static QuestManager* getInstance();
    static void purge();
    
    void init(QuestType type, const rapidjson::Value& jsonDict);
    void updateProgress(QuestType type, int questId, int progress);
    void getReward(QuestType type, int questId);
    
    // getters
    const std::vector<QuestData*>& getData(QuestType type);
    int getProgress(QuestType type, int questId) const;
    
protected:
    QuestManager();
    virtual ~QuestManager();
    QuestManager(const QuestManager &) = delete;
    QuestManager &operator =(const QuestManager &) = delete;
    void finish(QuestType type, int questId);
    
private:
    std::map<QuestType, std::vector<QuestData*>> _quests;
};

#endif /* QuestManager_h */
