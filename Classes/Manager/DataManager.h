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
#include "UAConfigData.h"
#include "SkllType.h"

class LevelLocalData;
class QuestLocalData;
class AchievementLocalData;
class GearLocalData;
class URConfigData;

class DataManager
{
public:
    static DataManager* getInstance();
    static void purge();
    
    void init();
    const LevelLocalData* getLevelData(int levelId) const;
    const QuestLocalData* getQuestData(QuestType type, int questId) const;
    const AchievementLocalData* getAchievementData(int achievementId) const;
    const GearLocalData* getGearData(int gearId) const;
    AnimationParameters getAnimationParameters(const std::string& name, UnderWorld::Core::SkillClass skillClass, UnitDirection direction) const;
    const URConfigData* getURConfigData(const std::string& name) const;
    
protected:
    DataManager();
    virtual ~DataManager();
    CC_DISALLOW_COPY_AND_ASSIGN(DataManager);
    
protected:
    void parseLevelData();
    void parseQuestData(QuestType type);
    void parseAchievementData();
    void parseGearData();
    void parseAnimationConfigData();
    void parseURConfigData();
    
private:
    std::map<int, LevelLocalData*> _levels;
    std::map<QuestType, std::map<int, QuestLocalData*>> _quests;
    std::map<int, AchievementLocalData*> _achievements;
    std::map<int, GearLocalData*> _gears;
    std::map<std::string, UAConfigData*> _animationParameters;
    std::map<std::string, URConfigData*> _unitResourceConfigData;
};

#endif /* DataManager_h */
