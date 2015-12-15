//
//  QuestManager.h
//  Underworld_Client
//
//  Created by Andy on 15/11/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef QuestManager_h
#define QuestManager_h

#include "cocos2d.h"
#include "json/document.h"
#include "CocosGlobal.h"

class QuestData;

class QuestManager
{
public:
    QuestManager();
    virtual ~QuestManager();
    
    void initQuest(QuestType type, const rapidjson::Value& jsonDict);
    void updateQuestProgress(QuestType type, int questId, int progress);
    void getReward(QuestType type, int questId);
    
    // getters
    const std::vector<QuestData*>& getQuestData(QuestType type);
    int getQuestProgress(QuestType type, int questId) const;
    
protected:
    CC_DISALLOW_COPY_AND_ASSIGN(QuestManager);
    void finishQuest(QuestType type, int questId);
    
private:
    std::map<QuestType, std::vector<QuestData*>> _quests;
};

#endif /* QuestManager_h */
