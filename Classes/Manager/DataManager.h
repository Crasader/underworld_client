//
//  DataManager.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef DataManager_h
#define DataManager_h

#include "cocos2d.h"
#include "CocosGlobal.h"
#include "AnimationConfigData.h"
#include "SkllType.h"

class QuestData;

class DataManager
{
public:
    static DataManager* getInstance();
    static void purge();
    
    void init();
    const QuestData* getQuestData(QuestType type, int questId) const;
    AnimationParameters getAnimationParameters(const std::string& name, UnderWorld::Core::SkillClass skillClass, UnitDirection direction);
    
protected:
    DataManager();
    virtual ~DataManager();
    CC_DISALLOW_COPY_AND_ASSIGN(DataManager);
    
protected:
    void parseQuestData(QuestType type);
    void parseAnimationConfigData();
    
private:
    std::map<QuestType, std::map<int, QuestData*>> _quests;
    std::map<std::string, AnimationConfigData*> _animationParameters;
};

#endif /* DataManager_h */
