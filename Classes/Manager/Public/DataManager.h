//
//  DataManager.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef DataManager_h
#define DataManager_h

#include "CocosGlobal.h"
#include "UAConfigData.h"
#include "SkllType.h"

class LevelLocalData;
class QuestLocalData;
class AchievementLocalData;
class ObjectLocalData;
class GearLocalData;
class GearUpgradeData;
class GearSetLocalData;
class URConfigData;
class MapParticleConfigData;
class AttributeLocalData;
class HeroLocalData;
class HeroUpgradeData;
class SkillLocalData;
class SoldierLocalData;
class SoldierUpgradeData;
class TowerLocalData;
class TowerUpgradeData;

class DataManager
{
public:
    static DataManager* getInstance();
    static void purge();
    
    void init();
    const LevelLocalData* getLevelData(int levelId) const;
    const QuestLocalData* getQuestData(QuestType type, int questId) const;
    const AchievementLocalData* getAchievementData(int achievementId) const;
    const ObjectLocalData* getObjectData(int objectId) const;
    const GearLocalData* getGearData(int gearId) const;
    const GearUpgradeData* getGearUpgradeData(int id, int level) const;
    const GearSetLocalData* getGearSetData(int id) const;
    AnimationParameters getAnimationParameters(const std::string& name, UnderWorld::Core::SkillClass skillClass, UnitDirection direction) const;
    const URConfigData* getURConfigData(const std::string& name) const;
    const std::vector<MapParticleConfigData*>& getMapParticleConfigData(int mapId) const;
    const AttributeLocalData* getAttributeData(int id) const;
    const HeroLocalData* getHeroData(int id) const;
    const HeroUpgradeData* getHeroUpgradeData(int id, int level) const;
    const SkillLocalData* getSkillData(int id) const;
    const SoldierLocalData* getSoldierData(int id) const;
    const SoldierUpgradeData* getSoldierUpgradeData(int id, int level) const;
    const TowerLocalData* getTowerData(int id) const;
    const TowerUpgradeData* getTowerUpgradeData(int id, int level) const;
    
protected:
    DataManager();
    virtual ~DataManager();
    M_DISALLOW_COPY_AND_ASSIGN(DataManager);
    
protected:
    void parseLevelData();
    void parseQuestData(QuestType type);
    void parseAchievementData();
    void parseObjectData();
    void parseGearData();
    void parseGearUpgradeData();
    void parseGearSetData();
    void parseAnimationConfigData();
    void parseURConfigData();
    void parseMapParticleConfigData();
    void parseAttributeLocalData();
    void parseHeroLocalData();
    void parseHeroUpgradeData();
    void parseSkillLocalData();
    void parseSoldierLocalData();
    void parseSoldierUpgradeData();
    void parseTowerLocalData();
    void parseTowerUpgradeData();
    
private:
    std::map<int, LevelLocalData*> _levels;
    std::map<QuestType, std::map<int, QuestLocalData*>> _quests;
    std::map<int, AchievementLocalData*> _achievements;
    std::map<int, ObjectLocalData*> _objects;
    std::map<int, GearLocalData*> _gears;
    std::map<std::string, GearUpgradeData*> _gearUpgradeDatas;
    std::map<int, GearSetLocalData*> _gearSets;
    std::map<std::string, UAConfigData*> _animationParameters;
    std::map<std::string, URConfigData*> _unitResourceConfigData;
    std::map<int, std::vector<MapParticleConfigData*>> _mapParticleConfigData;
    std::map<int, AttributeLocalData*> _attributes;
    std::map<int, HeroLocalData*> _heroes;
    std::map<std::string, HeroUpgradeData*> _heroUpgradeDatas;
    std::map<int, SkillLocalData*> _skills;
    std::map<int, SoldierLocalData*> _soldiers;
    std::map<std::string, SoldierUpgradeData*> _soldierUpgradeDatas;
    std::map<int, TowerLocalData*> _towers;
    std::map<std::string, TowerUpgradeData*> _towerUpgradeDatas;
};

#endif /* DataManager_h */
