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
#include "Unit.h"
#include "UAConfigData.h"
#include "SkllType.h"
#include <unordered_map>

class LevelLocalData;
class QuestLocalData;
class AchievementLocalData;
class ObjectLocalData;
class GearLocalData;
class GearUpgradeData;
class GearAttributeData;
class GearSetLocalData;
class CardConfigData;
class URConfigData;
class MapParticleConfigData;
class SpellConfigData;
class ArtifactLocalData;
class ArtifactUpgradeData;
class AttributeLocalData;
class HeroLocalData;
class HeroPieceData;
class HeroUpgradeData;
class SkillLocalData;
class SoldierLocalData;
class SoldierPieceData;
class SoldierUpgradeData;
class SoldierQualityData;
class SoldierTalentData;
class TowerLocalData;
class TowerUpgradeData;

class DataManager
{
public:
    static DataManager* getInstance();
    static void purge();
    
    void init();
    std::string getMapData(int mapId) const;
    std::string getTechTreeData(int mapId) const;
    
    const LevelLocalData* getLevelData(int levelId) const;
    const std::set<std::string>& getCardDecks() const;
    const QuestLocalData* getQuestData(QuestType type, int questId) const;
    const AchievementLocalData* getAchievementData(int achievementId) const;
    const ObjectLocalData* getObjectData(int objectId) const;
    
    // ---------- gear ----------
    const GearLocalData* getGearData(int gearId) const;
    const GearUpgradeData* getGearUpgradeData(int id, int level) const;
    const GearAttributeData* getGearAttributeData(int id, int level, int attributeId) const;
    const GearSetLocalData* getGearSetData(int id) const;
    
    // ---------- animations ----------
    const AnimationParameters& getAnimationParameters(const std::string& name, UnderWorld::Core::SkillClass skillClass, UnderWorld::Core::Unit::Direction direction) const;
    const CardConfigData* getCardConfigData(const std::string& name) const;
    const URConfigData* getURConfigData(const std::string& name) const;
    const std::vector<MapParticleConfigData*>& getMapParticleConfigData(int mapId) const;
    const SpellConfigData* getSpellConfigData(const std::string& name) const;
    
    // ---------- artifact ----------
    const ArtifactLocalData* getArtifactData(int id) const;
    const ArtifactUpgradeData* getArtifactUpgradeData(int id, int level) const;
    
    // ---------- attribute ----------
    const AttributeLocalData* getAttributeData(int id) const;
    
    // ---------- hero ----------
    const HeroLocalData* getHeroData(int id) const;
    const HeroPieceData* getHeroPieceData(int id) const;
    const HeroUpgradeData* getHeroUpgradeData(int id, int level) const;
    
    // ---------- skill ----------
    const SkillLocalData* getSkillData(int id) const;
    
    // ---------- soldier ----------
    const SoldierLocalData* getSoldierData(int id) const;
    const SoldierPieceData* getSoldierPieceData(int id) const;
    const SoldierUpgradeData* getSoldierUpgradeData(int id, int level) const;
    const SoldierQualityData* getSoldierQualityData(int id, int level) const;
    const SoldierTalentData* getSoldierTalentData(int id, int level) const;
    
    // ---------- tower ----------
    const TowerLocalData* getTowerData(int id) const;
    const TowerUpgradeData* getTowerUpgradeData(int id, int level) const;
    
protected:
    DataManager();
    virtual ~DataManager();
    M_DISALLOW_COPY_AND_ASSIGN(DataManager);
    
protected:
    void parseLevelData();
    void parseCardDecks();
    void parseQuestData(QuestType type);
    void parseAchievementData();
    void parseObjectData();
    void parseGearData();
    void parseGearUpgradeData();
    void parseGearAttributeData();
    void parseGearSetData();
    void parseAnimationConfigData();
    void parseCardConfigData();
    void parseURConfigData();
    void parseMapParticleConfigData();
    void parseSpellConfigData();
    void parseArtifactData();
    void parseArtifactUpgradeData();
    void parseAttributeData();
    void parseHeroData();
    void parseHeroPieceData();
    void parseHeroUpgradeData();
    void parseSkillData();
    void parseSoldierData();
    void parseSoldierPieceData();
    void parseSoldierUpgradeData();
    void parseSoldierQualityData();
    void parseSoldierTalentData();
    void parseTowerData();
    void parseTowerUpgradeData();
    
private:
    std::unordered_map<int, LevelLocalData*> _levels;
    std::set<std::string> _cardDecks;
    std::map<QuestType, std::unordered_map<int, QuestLocalData*>> _quests;
    std::unordered_map<int, AchievementLocalData*> _achievements;
    std::unordered_map<int, ObjectLocalData*> _objects;
    std::unordered_map<int, GearLocalData*> _gears;
    std::unordered_map<std::string, GearUpgradeData*> _gearUpgradeDatas;
    std::unordered_map<std::string, GearAttributeData*> _gearAttributeDatas;
    std::unordered_map<int, GearSetLocalData*> _gearSets;
    std::unordered_map<std::string, UAConfigData*> _animationParameters;
    std::unordered_map<std::string, CardConfigData*> _cardConfigData;
    std::unordered_map<std::string, URConfigData*> _unitResourceConfigData;
    std::unordered_map<int, std::vector<MapParticleConfigData*>> _mapParticleConfigData;
    std::unordered_map<std::string, SpellConfigData*> _spellConfigData;
    std::unordered_map<int, ArtifactLocalData*> _artifacts;
    std::unordered_map<std::string, ArtifactUpgradeData*> _artifactUpgradeData;
    std::unordered_map<int, AttributeLocalData*> _attributes;
    std::unordered_map<int, HeroLocalData*> _heroes;
    std::unordered_map<int, HeroPieceData*> _heroPieceDatas;
    std::unordered_map<std::string, HeroUpgradeData*> _heroUpgradeDatas;
    std::unordered_map<int, SkillLocalData*> _skills;
    std::unordered_map<int, SoldierLocalData*> _soldiers;
    std::unordered_map<int, SoldierPieceData*> _soldierPieceDatas;
    std::unordered_map<std::string, SoldierUpgradeData*> _soldierUpgradeDatas;
    std::unordered_map<std::string, SoldierQualityData*> _soldierQualityDatas;
    std::unordered_map<std::string, SoldierTalentData*> _soldierTalentDatas;
    std::unordered_map<int, TowerLocalData*> _towers;
    std::unordered_map<std::string, TowerUpgradeData*> _towerUpgradeDatas;
};

#endif /* DataManager_h */
